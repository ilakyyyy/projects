#include "head.h"

/**
 * Computes the squared norm of the input vector x.
 * 
 * sqnorm(x) = sum(x_i^2)
 */
float sqnorm(float* x, int size) {
    float sum;
    sum = 0;

    for(int i=0; i<size; i++) {
        sum += pow(*(x+i), 2); /*Equivalent to sum += x[i]^2*/
    }
    return(sum);
}

/**
 * Computes the root mean square (RMS) of the input vector x.
 * It adds a small epsilon for numerical stability.
 * 
 * rms(x) = sqrtf( (1/size) * sum(x_i^2) + epsilon )
 * 
 * where epsilon is a small constant (1e-5).
 * 
 * Note: use sqrtf instead of sqrt for float precision.
 */
float rms(float* x, int size) {
    float epsilon, rms;
    epsilon = 1e-5;

    rms = sqrtf(epsilon + (1.0/size) * sqnorm(x, size));
    return(rms);
}

/**
 * Computes the root mean square normalization (RMSNorm) of the input vector x
 * and stores the result in the output vector o. The normalization is scaled by
 * the provided weight vector.
 * 
 * o_i = weight_i * (x_i / rms(x)) 
 * 
 * where rms(x) = sqrtf( (1/size) * sum(x_j^2) + epsilon )
 */
void rmsnorm(float* o, float* x, float* weight, int size) {
    float RMS;
    RMS = rms(x,size);
    for(int i=0; i < size; i++) {
        *(o+i) = *(weight+i) * (*(x+i)/RMS);
    }
}

/**
 * Returns the maximum value in the input vector x.
 */
float max_val(float* x, int size) {
    float max; /*Asked Gemini whether adding "possmax" here and using it in the if and the assignation below would optimize speed, but it said it would be the same.*/
    max = *(x);
    for(int i=1; i < size; i++) {
        if (*(x+i) > max)
            max = *(x+i);
    }
    return (max);
}

/**
 * Applies the softmax function to the input vector x in place.
 * 
 * softmax(x_i) = expf(x_i - max) / sum_j expf(x_j - max)
 * 
 * where max is the maximum value in x (for numerical stability).
 * 
 * Note: use expf instead of exp for float precision.
 */
void softmax(float* x, int size) {
    float max, sum_j;
    max = max_val(x, size);
    sum_j = 0;

    for(int j=0; j < size; j++) {
        sum_j += expf(*(x+j) - max);
    }

    for(int i=0; i < size; i++) {
        *(x+i) = expf(*(x+i) - max) / sum_j;
    }
}

/**
 * Performs matrix multiplication between a quantized tensor x and a quantized weight tensor w.
 */
void matmul(float* xout, QuantizedTensor *x, QuantizedTensor *w, int n, int d) {
    int32_t val;
    float sum;
    int t;
    t = n/GS;

    for(int i=0; i < d; i++) {
        sum = 0;

        for(int j=0; j < t; j++) {
            val = 0;
            
            for(int k = 0; k < GS; k++) {
                val += (*x).q[j*GS+k] * (*w).q[i*n+j*GS+k];
            }
            sum += (float) val * (*x).s[j] * (*w).s[(i*t+j)];
        }
        *(xout+i) = sum;
    }
}

// ---------------------------------------------------------------------
// Do not modify anything below this line
// ---------------------------------------------------------------------

float* forward(Transformer* transformer, int token, int pos) {

    // a few convenience variables
    Config* p = &transformer->config;
    TransformerWeights* w = &transformer->weights;
    RunState* s = &transformer->state;
    float *x = s->x;
    int dim = p->dim;
    int kv_dim = (p->dim * p->n_kv_heads) / p->n_heads;
    int kv_mul = p->n_heads / p->n_kv_heads; // integer multiplier of the kv sharing in multiquery
    int hidden_dim =  p->hidden_dim;
    int head_size = dim / p->n_heads;

    // copy the token embedding into x
    memcpy(x, w->token_embedding_table + token*dim, dim * sizeof(float));

    // forward all the layers
    for(int l = 0; l < p->n_layers; l++) {

        // attention rmsnorm
        rmsnorm(s->xb, x, w->rms_att_weight + l*dim, dim);

        // qkv matmuls for this position
        quantize(&s->xq, s->xb, dim);
        matmul(s->q, &s->xq, w->wq + l, dim, dim);
        matmul(s->k, &s->xq, w->wk + l, dim, kv_dim);
        matmul(s->v, &s->xq, w->wv + l, dim, kv_dim);

        // RoPE relative positional encoding: complex-valued rotate q and k in each head
        for (int i = 0; i < dim; i+=2) {
            int head_dim = i % head_size;
            float freq = 1.0f / powf(10000.0f, head_dim / (float)head_size);
            float val = pos * freq;
            float fcr = cosf(val);
            float fci = sinf(val);
            int rotn = i < kv_dim ? 2 : 1; // how many vectors? 2 = q & k, 1 = q only
            for (int v = 0; v < rotn; v++) {
                float* vec = v == 0 ? s->q : s->k; // the vector to rotate (query or key)
                float v0 = vec[i];
                float v1 = vec[i+1];
                vec[i]   = v0 * fcr - v1 * fci;
                vec[i+1] = v0 * fci + v1 * fcr;
            }
        }

        // save key,value at this time step (pos) to our kv cache
        int loff = l * p->seq_len * kv_dim; // kv cache layer offset for convenience
        float* key_cache_row = s->key_cache + loff + pos * kv_dim;
        float* value_cache_row = s->value_cache + loff + pos * kv_dim;
        memcpy(key_cache_row, s->k, kv_dim * sizeof(*key_cache_row));
        memcpy(value_cache_row, s->v, kv_dim * sizeof(*value_cache_row));

        // multihead attention. iterate over all heads
        int h;
        #pragma omp parallel for private(h)
        for (h = 0; h < p->n_heads; h++) {
            // get the query vector for this head
            float* q = s->q + h * head_size;
            // attention scores for this head
            float* att = s->att + h * p->seq_len;
            // iterate over all timesteps, including the current one
            for (int t = 0; t <= pos; t++) {
                // get the key vector for this head and at this timestep
                float* k = s->key_cache + loff + t * kv_dim + (h / kv_mul) * head_size;
                // calculate the attention score as the dot product of q and k
                float score = 0.0f;
                for (int i = 0; i < head_size; i++) {
                    score += q[i] * k[i];
                }
                score /= sqrtf(head_size);
                // save the score to the attention buffer
                att[t] = score;
            }

            // softmax the scores to get attention weights, from 0..pos inclusively
            softmax(att, pos + 1);

            // weighted sum of the values, store back into xb
            float* xb = s->xb + h * head_size;
            memset(xb, 0, head_size * sizeof(float));
            for (int t = 0; t <= pos; t++) {
                // get the value vector for this head and at this timestep
                float* v = s->value_cache + loff + t * kv_dim + (h / kv_mul) * head_size;
                // get the attention weight for this timestep
                float a = att[t];
                // accumulate the weighted value into xb
                for (int i = 0; i < head_size; i++) {
                    xb[i] += a * v[i];
                }
            }
        }

        // final matmul to get the output of the attention
        quantize(&s->xq, s->xb, dim);
        matmul(s->xb2, &s->xq, w->wo + l, dim, dim);

        // residual connection back into x
        for (int i = 0; i < dim; i++) {
            x[i] += s->xb2[i];
        }

        // ffn rmsnorm
        rmsnorm(s->xb, x, w->rms_ffn_weight + l*dim, dim);

        // Now for FFN in PyTorch we have: self.w2(F.silu(self.w1(x)) * self.w3(x))
        // first calculate self.w1(x) and self.w3(x)
        quantize(&s->xq, s->xb, dim);
        matmul(s->hb, &s->xq, w->w1 + l, dim, hidden_dim);
        matmul(s->hb2, &s->xq, w->w3 + l, dim, hidden_dim);

        // SwiGLU non-linearity
        for (int i = 0; i < hidden_dim; i++) {
            float val = s->hb[i];
            // silu(x)=x*σ(x), where σ(x) is the logistic sigmoid
            val *= (1.0f / (1.0f + expf(-val)));
            // elementwise multiply with w3(x)
            val *= s->hb2[i];
            s->hb[i] = val;
        }

        // final matmul to get the output of the ffn
        quantize(&s->hq, s->hb, hidden_dim);
        matmul(s->xb, &s->hq, w->w2 + l, hidden_dim, dim);

        // residual connection
        for (int i = 0; i < dim; i++) {
            x[i] += s->xb[i];
        }
    }

    // final rmsnorm
    rmsnorm(x, x, w->rms_final_weight, dim);

    // classifier into logits
    quantize(&s->xq, x, dim);
    matmul(s->logits, &s->xq, w->wcls, dim, p->vocab_size);
    return s->logits;
}