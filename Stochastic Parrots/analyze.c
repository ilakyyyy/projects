#include "head.h"
#include <math.h>

void softmax(float* x, int size); // Ensure softmax is declared

/**
 * Calculates the log probability of a sequence of text given a prompt.
 */
double calculate_logprob(Transformer* transformer, Tokenizer* tokenizer, const char* prompt, const char* text) {
    // encode the prompt into tokens
    int num_prompt_tokens = 0;
    int* prompt_tokens = (int*)malloc((strlen(prompt) + 3) * sizeof(int));
    encode(tokenizer, (char*)prompt, 1, 0, prompt_tokens, &num_prompt_tokens);

    // encode the candidate text into tokens
    int num_text_tokens = 0;
    int* text_tokens = (int*)malloc((strlen(text) + 3) * sizeof(int));
    encode(tokenizer, (char*)text, 0, 0, text_tokens, &num_text_tokens);

    if (num_prompt_tokens < 1) {
        fprintf(stderr, "something is wrong, expected at least 1 prompt token\n");
        exit(EXIT_FAILURE);
    }

    // process the prompt tokens first to set the state
    int token = prompt_tokens[0];
    int pos = 0;
    while (pos < num_prompt_tokens) {
        forward(transformer, token, pos);
        token = prompt_tokens[pos + 1];
        pos++;
    }
    // at this point, pos = num_prompt_tokens, and the state is set up to predict the next token

    double total_log_prob = 0.0;
    printf("\n--- Analyzing text: \"%s\" ---\n", text);
    printf("%-5s %-20s %-15s\n", "Pos", "Token", "Log Probability");
    printf("--------------------------------------------\n");

    // now, calculate the probability of the candidate text, token by token
    for (int i = 0; i < num_text_tokens; i++) {
        // forward the transformer to get logits for the next token
        float* logits = forward(transformer, token, pos);

        // apply softmax to convert logits to probabilities
        softmax(logits, transformer->config.vocab_size);

        // the next token in the sequence we are evaluating
        int next_token = text_tokens[i];

        // find the probability of this token
        float prob = logits[next_token];
        double log_prob = log((double)prob);
        total_log_prob += log_prob;

        char* token_str = decode(tokenizer, token, next_token);
        printf("%-5d %-20s %-15.6f\n", pos, token_str, log_prob);

        // advance the state
        token = next_token;
        pos++;
    }

    free(prompt_tokens);
    free(text_tokens);

    return total_log_prob;
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    char *checkpoint_path = "models/tinyLlama.bin";
    char *tokenizer_path = "models/tokenizer.bin";
    char *prompt = "劉慈欣是位偉大的作家。宇宙是一片黑暗森林。";

    char *candidate1 = "宇宙就是一座黑暗森林，每个文明都是带枪的猎人";
    char *candidate2 = "劉是位大的作家。宇是一片森林。1. 如你是一位大的作家，你会样？A.";

    // build the Transformer via the model .bin file
    Transformer transformer;
    build_transformer(&transformer, checkpoint_path);

    // build the Tokenizer via the tokenizer .bin file
    Tokenizer tokenizer;
    build_tokenizer(&tokenizer, tokenizer_path, transformer.config.vocab_size);

    printf("Starting analysis with prompt: \"%s\"\n", prompt);

    double logprob1 = calculate_logprob(&transformer, &tokenizer, prompt, candidate1);
    printf("--------------------------------------------\n");
    printf("Total Log Probability for Candidate 1: %f\n\n", logprob1);

    // IMPORTANT: We must reset the transformer state to analyze the second candidate from the same starting point.
    // The easiest way is to rebuild it. A more efficient way would be to implement a reset_run_state function.
    free_transformer(&transformer);
    build_transformer(&transformer, checkpoint_path);

    double logprob2 = calculate_logprob(&transformer, &tokenizer, prompt, candidate2);
    printf("--------------------------------------------\n");
    printf("Total Log Probability for Candidate 2: %f\n\n", logprob2);

    printf("--- Conclusion ---\n");
    if (logprob1 > logprob2) {
        printf("Candidate 1 is more likely to be the generated text.\n");
    } else {
        printf("Candidate 2 is more likely to be the generated text.\n");
    }
    printf("LogProb(Cand1): %.4f, LogProb(Cand2): %.4f\n", logprob1, logprob2);

    // cleanup
    free_transformer(&transformer);
    free_tokenizer(&tokenizer);

    return 0;
}