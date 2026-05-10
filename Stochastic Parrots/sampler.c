#include "head.h"

/** 
 * Returns the index of the element with the highest probability.
 */
int sample_argmax(float* probabilities, int n) {
    int index;
    index = 0;
    for(int i=1; i<n; i++) {
        if ( *(probabilities+index) < *(probabilities+i) )
            index = i;
    }
    
    return (index);
}

/**
 * Sample an index from a probability distribution using the
 * cumulative distribution function (CDF).
 */
int sample_mult(float* probabilities, int n, float coin) {
    float acc;
    acc = 0.0;
    for(int i=0; i<n; i++) {
        acc += *(probabilities+i);
        if (acc > coin) {
            return(i);
        }
    }
    return(n-1); /*Just in case coin = 1 and acc = 0.9999...*/
}

/**
 * Samples the next token given the logits and the sampler configuration.
 * If the temperature is 0, performs greedy argmax sampling.
 * Otherwise, applies temperature scaling and samples from the resulting
 * probability distribution.
 */
int sample(Sampler* sampler, float* logits) {
    float temp, coin;
    temp = (*sampler).temperature;
    if (temp == 0.0) {
        return(sample_argmax(logits, (*sampler).vocab_size));
    }

    else {
        for(int i=0; i<(*sampler).vocab_size; i++) {
            *(logits+i) /= temp;
        }
        softmax(logits, (*sampler).vocab_size);

        coin = random_f32(&sampler->rng_state);
        return(sample_mult(logits, (*sampler).vocab_size, coin));
    }
}

// ---------------------------------------------------------------------
// Do not modify anything below this line
// ---------------------------------------------------------------------

int compare(const void* a, const void* b) {
    ProbIndex* a_ = (ProbIndex*) a;
    ProbIndex* b_ = (ProbIndex*) b;
    if (a_->prob > b_->prob) return -1;
    if (a_->prob < b_->prob) return 1;
    return 0;
}

int sample_topp(float* probabilities, int n, float topp, ProbIndex* probindex, float coin) {
    // top-p sampling (or "nucleus sampling") samples from the smallest set of
    // tokens that exceed probability topp. This way we never sample tokens that
    // have very low probabilities and are less likely to go "off the rails".
    // coin is a random number in [0, 1), usually from random_f32()

    int n0 = 0;
    // quicksort indices in descending order of probabilities
    // values smaller than (1 - topp) / (n - 1) cannot be part of the result
    // so for efficiency we crop these out as candidates before sorting
    const float cutoff = (1.0f - topp) / (n - 1);
    for (int i = 0; i < n; i++) {
        if (probabilities[i] >= cutoff) {
            probindex[n0].index = i;
            probindex[n0].prob = probabilities[i];
            n0++;
        }
    }
    qsort(probindex, n0, sizeof(ProbIndex), compare);

    // truncate the list where cumulative probability exceeds topp
    float cumulative_prob = 0.0f;
    int last_idx = n0 - 1; // in case of rounding errors consider all elements
    for (int i = 0; i < n0; i++) {
        cumulative_prob += probindex[i].prob;
        if (cumulative_prob > topp) {
            last_idx = i;
            break; // we've exceeded topp by including last_idx
        }
    }

    // sample from the truncated list
    float r = coin * cumulative_prob;
    float cdf = 0.0f;
    for (int i = 0; i <= last_idx; i++) {
        cdf += probindex[i].prob;
        if (r < cdf) {
            return probindex[i].index;
        }
    }
    return probindex[last_idx].index; // in case of rounding errors
}

void build_sampler(Sampler* sampler, int vocab_size, float temperature, float topp, unsigned long long rng_seed) {
    sampler->vocab_size = vocab_size;
    sampler->temperature = temperature;
    sampler->topp = topp;
    sampler->rng_state = rng_seed;
    // buffer only used with nucleus sampling; may not need but it's ~small
    sampler->probindex = malloc(sampler->vocab_size * sizeof(ProbIndex));
}

void free_sampler(Sampler* sampler) {
    free(sampler->probindex);
}

unsigned int random_u32(unsigned long long *state) {
    // xorshift rng: https://en.wikipedia.org/wiki/Xorshift#xorshift.2A
    *state ^= *state >> 12;
    *state ^= *state << 25;
    *state ^= *state >> 27;
    return (*state * 0x2545F4914F6CDD1Dull) >> 32;
}
float random_f32(unsigned long long *state) { // random float32 in [0,1)
    return (random_u32(state) >> 8) / 16777216.0f;
}