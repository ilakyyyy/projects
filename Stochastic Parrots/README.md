# Stochastic Parrots

This is an implementation of Llama 2 in C.

## Compilation

To compile the code using Visual Studio Code:

1. Open the project in VS Code.
2. Open the Command Palette (Ctrl+Shift+P) and select "Tasks: Run Task".
3. Choose "build parrot" to compile the main executable.

## Model Weights

You need to download the weights (matrices) of a model compatible with the Llama 2 architecture. You can download these examples (put them in the `models/` folder):

- **[tinyLlama](https://github.com/jzhang38/TinyLlama)**: A small language model trained on 3 trillion tokens. [Click here to download](https://drive.google.com/file/d/1XR5-9eEKBxLLy4e920RDT-eG6Pnhr9d5/view?usp=sharing)
- **[tinyStories](https://huggingface.co/datasets/roneneldan/TinyStories)**: A compact model trained on children's stories. [Click here to download](https://drive.google.com/file/d/1SX1IqGOnbxysBGbNpcrqzp8R-oboQ5dH/view?usp=sharing)

## Running the Code

You have to open a terminal in the project directory and run the executable with the appropriate parameters.

### Generate Mode

In generate mode, the model generates text based on a prompt.

```bash
./parrot.exe <model> -m generate -i "Your prompt here" [extra options]
./parrot.exe models/stories110M.bin -m generate -i "Math is boring, physics is much" -t 0 -n 47
./parrot.exe models/tinyLlama.bin -m generate -i "I think that I will pass the exam, but" -t 1 -n 60 -s 924640
```

### Chat Mode

In chat mode, the model engages in a conversation.

```bash
./parrot.exe <model> -m chat -i "Initial message" -y "System prompt" [extra options]
```

### Parameters

- `<model>`: Path to the model checkpoint file (e.g., `models/tinyLlama.bin`)
- `-t <float>`: Temperature for sampling (default 1.0). Lower values make output more deterministic.
- `-s <int>`: Random seed (default: current time)
- `-n <int>`: Number of steps to generate (default 256). 0 means maximum sequence length.
- `-i <string>`: Input prompt
- `-m <string>`: Mode: `generate` or `chat` (default: `generate`)
- `-y <string>`: System prompt for chat mode (optional)
