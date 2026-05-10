#include "../head.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define RUN_TEST(test_function) do { \
    n_test++; \
    printf("%d) ", n_test); \
    status = test_function(); \
    if(status) { \
        printf("OK\n"); \
        passed++; \
    } \
} while(0)

#define EPSILON 1e-3
#define APPROX_EQUAL(x, y) (fabs((x) - (y)) < EPSILON)

bool test_sqnorm() {
    printf("Testing sqnorm...");

    // Test with positive numbers
    float x1[] = {1.0f, 2.0f, 3.0f};
    float result = sqnorm(x1, 3);
    if (!APPROX_EQUAL(result, 14.0f)) {
        printf("\n\tERROR: For [1,2,3], expected 14.0, got %f\n", result);
        return false;
    }

    // Test with zeros
    float x2[] = {0.0f, 0.0f, 0.0f};
    result = sqnorm(x2, 3);
    if (!APPROX_EQUAL(result, 0.0f)) {
        printf("\n\tERROR: For [0,0,0], expected 0.0, got %f\n", result);
        return false;
    }

    // Test with negative numbers
    float x3[] = {-1.0f, 1.0f};
    result = sqnorm(x3, 2);
    if (!APPROX_EQUAL(result, 2.0f)) {
        printf("\n\tERROR: For [-1,1], expected 2.0, got %f\n", result);
        return false;
    }

    // Test with size 1
    float x4[] = {5.0f};
    result = sqnorm(x4, 1);
    if (!APPROX_EQUAL(result, 25.0f)) {
        printf("\n\tERROR: For [5], expected 25.0, got %f\n", result);
        return false;
    }

    // Test with mixed numbers
    float x5[] = {-2.0f, 3.0f, -4.0f};
    result = sqnorm(x5, 3);
    if (!APPROX_EQUAL(result, 29.0f)) {
        printf("\n\tERROR: For [-2,3,-4], expected 29.0, got %f\n", result);
        return false;
    }

    return true;
}

bool test_rms() {
    printf("Testing rms...");

    // Test with uniform values
    float x1[] = {1.0f, 1.0f, 1.0f, 1.0f}; // size 4, sum_sq=4, mean_sq=1, rms=sqrt(1+1e-5)≈1.000005
    float result = rms(x1, 4);
    float expected = sqrtf(1.0f + 1e-5f);
    if (!APPROX_EQUAL(result, expected)) {
        printf("\n\tERROR: For [1,1,1,1], expected %f, got %f\n", expected, result);
        return false;
    }

    // Test with zeros
    float x2[] = {0.0f, 0.0f};
    result = rms(x2, 2);
    expected = sqrtf(0.0f + 1e-5f);
    if (!APPROX_EQUAL(result, expected)) {
        printf("\n\tERROR: For [0,0], expected %f, got %f\n", expected, result);
        return false;
    }

    // Test with size 1
    float x3[] = {4.0f};
    result = rms(x3, 1);
    expected = sqrtf(16.0f + 1e-5f);
    if (!APPROX_EQUAL(result, expected)) {
        printf("\n\tERROR: For [4], expected %f, got %f\n", expected, result);
        return false;
    }

    // Test with negative values
    float x4[] = {-2.0f, 2.0f};
    result = rms(x4, 2);
    expected = sqrtf(4.0f + 1e-5f);
    if (!APPROX_EQUAL(result, expected)) {
        printf("\n\tERROR: For [-2,2], expected %f, got %f\n", expected, result);
        return false;
    }

    return true;
}

bool test_rmsnorm() {
    printf("Testing rmsnorm...");

    // First test case
    float x[] = {1.0f, 2.0f, 3.0f};
    float weight[] = {0.5f, 1.0f, 1.5f};
    float o[3];
    rmsnorm(o, x, weight, 3);

    // rms of x: sqrt((1+4+9)/3 + 1e-5) = sqrt(4.666666 + 0.00001) ≈ 2.160246
    float rms_val = rms(x, 3);
    float expected_o[3] = {weight[0] * x[0] / rms_val, weight[1] * x[1] / rms_val, weight[2] * x[2] / rms_val};

    for (int i = 0; i < 3; i++) {
        if (!APPROX_EQUAL(o[i], expected_o[i])) {
            printf("\n\tERROR: at index %d, expected %f, got %f\n", i, expected_o[i], o[i]);
            return false;
        }
    }

    // Second test case with different values
    float x2[] = {0.0f, 1.0f};
    float weight2[] = {2.0f, 0.5f};
    float o2[2];
    rmsnorm(o2, x2, weight2, 2);

    rms_val = rms(x2, 2);
    float expected_o2[2] = {weight2[0] * x2[0] / rms_val, weight2[1] * x2[1] / rms_val};

    for (int i = 0; i < 2; i++) {
        if (!APPROX_EQUAL(o2[i], expected_o2[i])) {
            printf("\n\tERROR: at index %d (second test), expected %f, got %f\n", i, expected_o2[i], o2[i]);
            return false;
        }
    }

    return true;
}

bool test_max_val() {
    printf("Testing max_val...");

    // Test with positive numbers
    float x1[] = {1.0f, 5.0f, 3.0f, 2.0f};
    float result = max_val(x1, 4);
    if (!APPROX_EQUAL(result, 5.0f)) {
        printf("\n\tERROR: For [1,5,3,2], expected 5.0, got %f\n", result);
        return false;
    }

    // Test with negative numbers
    float x2[] = {-1.0f, -5.0f, -3.0f};
    result = max_val(x2, 3);
    if (!APPROX_EQUAL(result, -1.0f)) {
        printf("\n\tERROR: For [-1,-5,-3], expected -1.0, got %f\n", result);
        return false;
    }

    // Test with single element
    float x3[] = {42.0f};
    result = max_val(x3, 1);
    if (!APPROX_EQUAL(result, 42.0f)) {
        printf("\n\tERROR: For [42], expected 42.0, got %f\n", result);
        return false;
    }

    // Test with mixed positive and negative
    float x4[] = {-10.0f, 0.0f, 15.0f, -5.0f};
    result = max_val(x4, 4);
    if (!APPROX_EQUAL(result, 15.0f)) {
        printf("\n\tERROR: For [-10,0,15,-5], expected 15.0, got %f\n", result);
        return false;
    }

    // Test with duplicates
    float x5[] = {3.0f, 3.0f, 3.0f};
    result = max_val(x5, 3);
    if (!APPROX_EQUAL(result, 3.0f)) {
        printf("\n\tERROR: For [3,3,3], expected 3.0, got %f\n", result);
        return false;
    }

    return true;
}

bool test_softmax() {
    printf("Testing softmax...");

    // Test with simple values
    float x1[] = {1.0f, 2.0f, 3.0f};
    softmax(x1, 3);
    float sum = 0.0f;
    for (int i = 0; i < 3; i++) sum += x1[i];
    if (!APPROX_EQUAL(sum, 1.0f)) {
        printf("\n\tERROR: Sum should be 1.0, got %f\n", sum);
        return false;
    }
    // Check specific values approximately
    if (!APPROX_EQUAL(x1[0], 0.09003057f) || !APPROX_EQUAL(x1[1], 0.24472847f) || !APPROX_EQUAL(x1[2], 0.66524096f)) {
        printf("\n\tERROR: Incorrect softmax values\n");
        return false;
    }

    // Test with zeros
    float x2[] = {0.0f, 0.0f};
    softmax(x2, 2);
    sum = x2[0] + x2[1];
    if (!APPROX_EQUAL(sum, 1.0f) || !APPROX_EQUAL(x2[0], 0.5f)) {
        printf("\n\tERROR: For [0,0], expected [0.5,0.5], got [%f,%f]\n", x2[0], x2[1]);
        return false;
    }

    // Test with single element
    float x3[] = {5.0f};
    softmax(x3, 1);
    if (!APPROX_EQUAL(x3[0], 1.0f)) {
        printf("\n\tERROR: For [5], expected 1.0, got %f\n", x3[0]);
        return false;
    }

    return true;
}

bool test_matmul() {
    printf("Testing matmul...");

    extern int GS;
    int old_gs = GS;
    GS = 2; // temporarily set

    // Test case 1: Identity matrix
    QuantizedTensor x1;
    int8_t x1_q[] = {1, 2};
    float x1_s[] = {1.0f};
    x1.q = x1_q;
    x1.s = x1_s;

    QuantizedTensor w1;
    int8_t w1_q[] = {1, 0, 0, 1};
    float w1_s[] = {1.0f, 1.0f};
    w1.q = w1_q;
    w1.s = w1_s;

    float xout1[2];
    matmul(xout1, &x1, &w1, 2, 2);

    if (!APPROX_EQUAL(xout1[0], 1.0f) || !APPROX_EQUAL(xout1[1], 2.0f)) {
        printf("\n\tERROR: Test 1 - Expected [1,2], got [%f,%f]\n", xout1[0], xout1[1]);
        GS = old_gs;
        return false;
    }

    // Test case 2: Simple matrix [[2,1],[1,2]] * [1,1] = [3,3]
    QuantizedTensor x2;
    int8_t x2_q[] = {1, 1};
    float x2_s[] = {1.0f};
    x2.q = x2_q;
    x2.s = x2_s;

    QuantizedTensor w2;
    int8_t w2_q[] = {2, 1, 1, 2};
    float w2_s[] = {1.0f, 1.0f};
    w2.q = w2_q;
    w2.s = w2_s;

    float xout2[2];
    matmul(xout2, &x2, &w2, 2, 2);

    if (!APPROX_EQUAL(xout2[0], 3.0f) || !APPROX_EQUAL(xout2[1], 3.0f)) {
        printf("\n\tERROR: Test 2 - Expected [3,3], got [%f,%f]\n", xout2[0], xout2[1]);
        GS = old_gs;
        return false;
    }

    // Test case 3: With scales - x scaled by 0.25, w by 2.0
    QuantizedTensor x3;
    int8_t x3_q[] = {2, 4}; // represents [1,2] with scale 0.25
    float x3_s[] = {0.25f};
    x3.q = x3_q;
    x3.s = x3_s;

    QuantizedTensor w3;
    int8_t w3_q[] = {1, 0, 0, 1}; // identity, but scale 2.0
    float w3_s[] = {2.0f, 2.0f};
    w3.q = w3_q;
    w3.s = w3_s;

    float xout3[2];
    matmul(xout3, &x3, &w3, 2, 2);

    // Expected: (identity * 2) @ (x * 0.25) = 2 * [0.5,1] = [1,2]
    if (!APPROX_EQUAL(xout3[0], 1.0f) || !APPROX_EQUAL(xout3[1], 2.0f)) {
        printf("\n\tERROR: Test 3 - Expected [1,2], got [%f,%f]\n", xout3[0], xout3[1]);
        GS = old_gs;
        return false;
    }

    // Test case 4: Larger dimension, n=4, d=2
    GS = 2; // ensure GS=2
    QuantizedTensor x4;
    int8_t x4_q[] = {1, 2, 3, 4};
    float x4_s[] = {1.0f, 1.0f}; // two groups
    x4.q = x4_q;
    x4.s = x4_s;

    QuantizedTensor w4;
    int8_t w4_q[] = {1, 0, 0, 0, 0, 1, 0, 0}; // first row [1,0,0,0], second [0,1,0,0]
    float w4_s[] = {1.0f, 1.0f, 0.5f, 1.0f}; // four groups
    w4.q = w4_q;
    w4.s = w4_s;

    float xout4[2];
    matmul(xout4, &x4, &w4, 4, 2);

    // Expected: [[1,0,0,0],[0,1,0,0]] @ [1,2,3,4] = [1,1]
    if (!APPROX_EQUAL(xout4[0], 1.0f) || !APPROX_EQUAL(xout4[1], 1.0f)) {
        printf("\n\tERROR: Test 4 - Expected [1,1], got [%f,%f]\n", xout4[0], xout4[1]);
        GS = old_gs;
        return false;
    }

    GS = old_gs; // restore
    return true;
}

bool test_sample_argmax() {
    printf("Testing sample_argmax...");

    // Test case 1: Maximum at the beginning
    float probs1[] = {0.8f, 0.1f, 0.05f, 0.05f};
    int result1 = sample_argmax(probs1, 4);
    if (result1 != 0) {
        printf("\n\tERROR: For [0.8,0.1,0.05,0.05], expected index 0, got %d\n", result1);
        return false;
    }

    // Test case 2: Maximum in the middle
    float probs2[] = {0.2f, 0.5f, 0.2f, 0.1f};
    int result2 = sample_argmax(probs2, 4);
    if (result2 != 1) {
        printf("\n\tERROR: For [0.2,0.5,0.2,0.1], expected index 1, got %d\n", result2);
        return false;
    }

    // Test case 3: Maximum at the end
    float probs3[] = {0.1f, 0.2f, 0.3f, 0.4f};
    int result3 = sample_argmax(probs3, 4);
    if (result3 != 3) {
        printf("\n\tERROR: For [0.1,0.2,0.3,0.4], expected index 3, got %d\n", result3);
        return false;
    }

    // Test case 4: All equal values
    float probs4[] = {0.25f, 0.25f, 0.25f, 0.25f};
    int result4 = sample_argmax(probs4, 4);
    if (result4 != 0) {
        printf("\n\tERROR: For equal probabilities, expected index 0, got %d\n", result4);
        return false;
    }

    return true;
}

bool test_sample_mult() {
    printf("Testing sample_mult...");

    // Test with probabilities [0.5, 0.3, 0.2]
    float probs[] = {0.5f, 0.3f, 0.2f};

    // coin < 0.5 should return 0
    int result1 = sample_mult(probs, 3, 0.4f);
    if (result1 != 0) {
        printf("\n\tERROR: For coin=0.4, expected index 0, got %d\n", result1);
        return false;
    }

    // 0.5 <= coin < 0.8 should return 1
    int result2 = sample_mult(probs, 3, 0.6f);
    if (result2 != 1) {
        printf("\n\tERROR: For coin=0.6, expected index 1, got %d\n", result2);
        return false;
    }

    // 0.8 <= coin < 1.0 should return 2
    int result3 = sample_mult(probs, 3, 0.9f);
    if (result3 != 2) {
        printf("\n\tERROR: For coin=0.9, expected index 2, got %d\n", result3);
        return false;
    }

    // Test edge case: coin exactly at boundary
    int result4 = sample_mult(probs, 3, 0.5f);
    if (result4 != 1) {
        printf("\n\tERROR: For coin=0.5, expected index 1, got %d\n", result4);
        return false;
    }

    // Test with different probabilities [0.1, 0.9]
    float probs2[] = {0.1f, 0.9f};
    int result5 = sample_mult(probs2, 2, 0.05f);
    if (result5 != 0) {
        printf("\n\tERROR: For [0.1,0.9] with coin=0.05, expected index 0, got %d\n", result5);
        return false;
    }

    int result6 = sample_mult(probs2, 2, 0.15f);
    if (result6 != 1) {
        printf("\n\tERROR: For [0.1,0.9] with coin=0.15, expected index 1, got %d\n", result6);
        return false;
    }

    return true;
}

bool test_sample() {
    printf("Testing sample...");

    // Test case 1: Temperature 0 (greedy sampling)
    Sampler sampler_greedy;
    build_sampler(&sampler_greedy, 4, 0.0f, 1.0f, 42); // temperature 0, topp 1.0, seed 42

    float logits1[] = {1.0f, 3.0f, 2.0f, 0.5f}; // max at index 1
    float logits1_orig[4];
    memcpy(logits1_orig, logits1, sizeof(logits1)); // save original

    int result1 = sample(&sampler_greedy, logits1);
    if (result1 != 1) {
        printf("\n\tERROR: Greedy sampling with max at index 1, got %d\n", result1);
        free_sampler(&sampler_greedy);
        return false;
    }

    // Check that logits are unchanged for temperature 0
    for (int i = 0; i < 4; i++) {
        if (!APPROX_EQUAL(logits1[i], logits1_orig[i])) {
            printf("\n\tERROR: Logits changed for temperature 0 at index %d\n", i);
            free_sampler(&sampler_greedy);
            return false;
        }
    }

    free_sampler(&sampler_greedy);

    // Test case 2: Temperature > 0 (probabilistic sampling)
    Sampler sampler_temp;
    build_sampler(&sampler_temp, 4, 2.0f, 1.0f, 123); // temperature 2.0, topp 1.0, seed 123

    float logits2[] = {1.0f, 3.0f, 2.0f, 0.5f};
    float logits2_orig[4];
    memcpy(logits2_orig, logits2, sizeof(logits2)); // save original

    int result2 = sample(&sampler_temp, logits2);
    if (result2 < 0 || result2 >= 4) {
        printf("\n\tERROR: Invalid sample index %d\n", result2);
        free_sampler(&sampler_temp);
        return false;
    }

    // Check that logits are transformed to probabilities (sum to 1)
    float sum = 0.0f;
    for (int i = 0; i < 4; i++) {
        sum += logits2[i];
        if (logits2[i] < 0.0f || logits2[i] > 1.0f) {
            printf("\n\tERROR: Logit at index %d is not a valid probability: %f\n", i, logits2[i]);
            free_sampler(&sampler_temp);
            return false;
        }
    }
    if (!APPROX_EQUAL(sum, 1.0f)) {
        printf("\n\tERROR: Probabilities don't sum to 1, sum = %f\n", sum);
        free_sampler(&sampler_temp);
        return false;
    }

    // Check that logits changed from original (divided by temperature and softmax applied)
    bool changed = false;
    for (int i = 0; i < 4; i++) {
        if (!APPROX_EQUAL(logits2[i], logits2_orig[i] / 2.0f)) { // should be different after softmax
            changed = true;
            break;
        }
    }
    if (!changed) {
        printf("\n\tERROR: Logits not properly transformed for temperature > 0\n");
        free_sampler(&sampler_temp);
        return false;
    }

    free_sampler(&sampler_temp);

    return true;
}

int main() {
    int passed = 0, n_test = 0;
    bool status = true;

    printf("**********************\n* TESTING THE PARROT *\n**********************\n\n");
    RUN_TEST(test_sqnorm);
    RUN_TEST(test_rms);
    RUN_TEST(test_rmsnorm);
    RUN_TEST(test_max_val);
    RUN_TEST(test_softmax);
    RUN_TEST(test_matmul);
    RUN_TEST(test_sample_argmax);
    RUN_TEST(test_sample_mult);
    RUN_TEST(test_sample);

    printf("\nTests succesfully completed: %d/%d\n", passed, n_test);

    return n_test - passed;
}