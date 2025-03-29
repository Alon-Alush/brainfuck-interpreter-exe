#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <io.h>  // For _isatty and _fileno on Windows

// Configurable parameters
#define DEFAULT_MEMORY_SIZE 30000
#define MAX_NESTED_LOOPS 1000
#define MAX_PROGRAM_SIZE 1000000
#define INPUT_BUFFER_SIZE 4096

typedef struct {
    bool wrap_memory;        // If true, wrap around memory instead of bounds checking
    bool debug_mode;         // If true, print debug information
    unsigned int memory_size; // Size of the memory tape
    bool eof_behavior;       // If true, set cell to 0 on EOF, otherwise don't change
} BrainfuckConfig;

// Debug function to print memory state around the current pointer
void print_debug_state(unsigned char* memory, unsigned char* ptr,
    unsigned int memory_size, size_t pc, char instruction) {
    int context = 10; // Show this many cells around the current pointer

    printf("\n[DEBUG] PC: %zu, Instruction: %c\n", pc, instruction);

    // Calculate range to display
    size_t ptr_pos = ptr - memory;
    size_t start = (ptr_pos > context) ? ptr_pos - context : 0;
    size_t end = (ptr_pos + context < memory_size) ? ptr_pos + context : memory_size - 1;

    printf("Memory[%zu-%zu]: ", start, end);
    for (size_t i = start; i <= end; i++) {
        if (i == ptr_pos) {
            printf("[%d] ", memory[i]); // Highlight current pointer
        }
        else {
            printf("%d ", memory[i]);
        }
    }
    printf("\n");
}

// Function to execute brainfuck code with configuration
void execute_brainfuck(char* code, BrainfuckConfig config) {
    // Allocate memory for the tape
    unsigned char* memory = (unsigned char*)calloc(config.memory_size, sizeof(unsigned char));
    if (!memory) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }

    unsigned char* ptr = memory; // Data pointer

    // Program counter
    size_t pc = 0;
    size_t code_length = strlen(code);

    // Stack to keep track of loop positions
    size_t* loop_stack = (size_t*)malloc(MAX_NESTED_LOOPS * sizeof(size_t));
    if (!loop_stack) {
        fprintf(stderr, "Error: Memory allocation failed for loop stack\n");
        free(memory);
        return;
    }

    size_t stack_pos = 0;

    // Input buffer setup
    char input_buffer[INPUT_BUFFER_SIZE];
    size_t input_pos = 0;
    size_t input_size = 0;

    // Execute the code
    while (pc < code_length) {
        char instruction = code[pc];

        if (config.debug_mode) {
            print_debug_state(memory, ptr, config.memory_size, pc, instruction);
        }

        switch (instruction) {
        case '>': // Increment data pointer
            if (config.wrap_memory) {
                ptr = (ptr == memory + config.memory_size - 1) ? memory : ptr + 1;
            }
            else if (ptr < memory + config.memory_size - 1) {
                ptr++;
            }
            else {
                fprintf(stderr, "Error: Data pointer out of bounds at position %zu\n", pc);
                free(memory);
                free(loop_stack);
                return;
            }
            break;

        case '<': // Decrement data pointer
            if (config.wrap_memory) {
                ptr = (ptr == memory) ? memory + config.memory_size - 1 : ptr - 1;
            }
            else if (ptr > memory) {
                ptr--;
            }
            else {
                fprintf(stderr, "Error: Data pointer out of bounds at position %zu\n", pc);
                free(memory);
                free(loop_stack);
                return;
            }
            break;

        case '+': // Increment value at data pointer
            (*ptr)++;
            break;

        case '-': // Decrement value at data pointer
            (*ptr)--;
            break;

        case '.': // Output value at data pointer
            putchar(*ptr);
            fflush(stdout);
            break;

        case ',': // Input value and store at data pointer
            // If input buffer is empty or we've used all buffered input, refill it
            if (input_pos >= input_size) {
                input_size = 0;
                input_pos = 0;

                // Prompt for input only in interactive mode
                if (_isatty(_fileno(stdin))) {
                    printf("\nInput: ");
                    fflush(stdout);
                }

                if (fgets(input_buffer, INPUT_BUFFER_SIZE, stdin) != NULL) {
                    input_size = strlen(input_buffer);
                }
            }

            // Handle input
            if (input_pos < input_size) {
                *ptr = (unsigned char)input_buffer[input_pos++];
            }
            else {
                // EOF condition
                if (config.eof_behavior) {
                    *ptr = 0; // Set to 0 on EOF
                }
                // Otherwise leave the cell unchanged
            }
            break;

        case '[': // Start of loop
            if (*ptr == 0) {
                // Skip to matching ']'
                size_t nest_level = 1;
                while (nest_level > 0) {
                    pc++;
                    if (pc >= code_length) {
                        fprintf(stderr, "Error: Unmatched '[' at position %zu\n",
                            stack_pos > 0 ? loop_stack[stack_pos - 1] : pc);
                        free(memory);
                        free(loop_stack);
                        return;
                    }
                    if (code[pc] == '[') {
                        nest_level++;
                    }
                    else if (code[pc] == ']') {
                        nest_level--;
                    }
                }
            }
            else {
                // Push current position onto stack
                if (stack_pos >= MAX_NESTED_LOOPS) {
                    fprintf(stderr, "Error: Too many nested loops (max %d)\n", MAX_NESTED_LOOPS);
                    free(memory);
                    free(loop_stack);
                    return;
                }
                loop_stack[stack_pos++] = pc;
            }
            break;

        case ']': // End of loop
            if (stack_pos <= 0) {
                fprintf(stderr, "Error: Unmatched ']' at position %zu\n", pc);
                free(memory);
                free(loop_stack);
                return;
            }

            if (*ptr != 0) {
                // Jump back to matching '['
                pc = loop_stack[stack_pos - 1];
            }
            else {
                // Exit the loop
                stack_pos--;
            }
            break;
        }

        pc++;
    }

    // Check if all loops are closed
    if (stack_pos != 0) {
        fprintf(stderr, "Error: %zu unclosed loops\n", stack_pos);
    }

    // Free the allocated memory
    free(memory);
    free(loop_stack);
}

// Function to filter out non-brainfuck characters
char* clean_code(const char* input) {
    size_t input_len = strlen(input);
    char* cleaned = (char*)malloc(input_len + 1);
    if (!cleaned) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    size_t j = 0;
    for (size_t i = 0; i < input_len; i++) {
        char c = input[i];
        if (c == '>' || c == '<' || c == '+' || c == '-' ||
            c == '.' || c == ',' || c == '[' || c == ']') {
            cleaned[j++] = c;
        }
    }
    cleaned[j] = '\0';
    return cleaned;
}

void print_usage(const char* program_name) {
    printf("Usage: %s [options] <brainfuck_file>\n\n", program_name);
    printf("Options:\n");
    printf("  -w           Enable memory wrapping (instead of bounds checking)\n");
    printf("  -d           Enable debug mode\n");
    printf("  -m <size>    Set memory size (default: %d)\n", DEFAULT_MEMORY_SIZE);
    printf("  -z           Set cell to 0 on EOF (default: leave unchanged)\n");
    printf("\nExample: %s -w -m 100000 program.bf\n", program_name);
    system("pause");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Default configuration
    BrainfuckConfig config = {
        .wrap_memory = false,
        .debug_mode = false,
        .memory_size = DEFAULT_MEMORY_SIZE,
        .eof_behavior = false
    };

    // Parse command line options
    int filename_arg = 1;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // This is an option
            switch (argv[i][1]) {
            case 'w':
                config.wrap_memory = true;
                break;
            case 'd':
                config.debug_mode = true;
                break;
            case 'm':
                if (i + 1 < argc) {
                    config.memory_size = atoi(argv[i + 1]);
                    if (config.memory_size == 0) {
                        config.memory_size = DEFAULT_MEMORY_SIZE;
                    }
                    i++; // Skip the next argument (the memory size)
                }
                break;
            case 'z':
                config.eof_behavior = true;
                break;
            default:
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
            filename_arg = i + 1;
        }
        else {
            // First non-option argument is the filename
            filename_arg = i;
            break;
        }
    }

    if (filename_arg >= argc) {
        fprintf(stderr, "Error: No brainfuck file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    // Allocate program buffer
    char* program = (char*)malloc(MAX_PROGRAM_SIZE * sizeof(char));
    if (!program) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    // Read from file
    FILE* file = NULL;
    errno_t err = fopen_s(&file, argv[filename_arg], "r");
    if (err != 0 || !file) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[filename_arg]);
        free(program);
        return 1;
    }

    // Read the entire file
    size_t bytesRead = fread(program, 1, MAX_PROGRAM_SIZE - 1, file);
    program[bytesRead] = '\0';
    fclose(file);

    // Clean the code
    char* cleaned_code = clean_code(program);
    free(program);

    printf("Running Brainfuck program from: %s\n", argv[filename_arg]);
    printf("Configuration: Memory Size=%u, Wrapping=%s, Debug=%s, EOF=Set to %s\n\n",
        config.memory_size,
        config.wrap_memory ? "Enabled" : "Disabled",
        config.debug_mode ? "Enabled" : "Disabled",
        config.eof_behavior ? "0" : "Unchanged");

    execute_brainfuck(cleaned_code, config);
    printf("\n\nProgram execution complete.\n");

    free(cleaned_code);

    // Keep console window open if running in a terminal
    if (_isatty(_fileno(stdin))) {
        printf("Press Enter to exit...");
        // Use a return value check to address the warning
        int ch = getchar();
        (void)ch; // Suppress unused variable warning
    }

    return 0;
}
