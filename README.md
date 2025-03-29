# Brainfuck Interpreter

```
____            *        *_            *    *____       *                           *            
|  * \          (*)      / *|          | |  |*   _|     | |                         | |           
| |_) |_ ** *** _* | |_ *   *  ___| | __ | |  *_* | |_ ___ *_* *_*  *_* ___| |_ ___ *_* 
|  * <| '*_/ *` | | '* \|  *| | | |/ *_| |/ / | | | '_ \| __/ * \ '*_| '_ \| '__/ * \ *_/ * \ '*_|
| |_) | | | (_| | | | | | | | |_| | (__|   < *| |*| | | | ||  __/ |  | |_) | | |  **/ ||  **/ |   
|____/|_|  \__,_|_|_| |_|_|  \__,_|\___|_|\_\_____|_| |_|\__\___|_|  | .__/|_|  \___|\__\___|_|   
                                                                     | |                          
                                                                     |_|                          
```

## Overview

Welcome to the Brainfuck Interpreter, a powerful and flexible tool for running Brainfuck programs. This interpreter supports standard Brainfuck commands along with various configuration options to handle complex programs.

**Created by:** Alon Alush  
**Email:** alonalush5@gmail.com

## What is Brainfuck?

Brainfuck is an esoteric programming language with just eight commands, designed to challenge programmers with its minimalism. Despite its simplicity, it's Turing-complete and can theoretically compute anything a conventional programming language can.

## Commands

The Brainfuck language consists of these eight commands:

| Command | Description |
|---------|-------------|
| `>` | Increment the data pointer |
| `<` | Decrement the data pointer |
| `+` | Increment the byte at the data pointer |
| `-` | Decrement the byte at the data pointer |
| `.` | Output the byte at the data pointer as an ASCII character |
| `,` | Accept one byte of input and store it at the data pointer |
| `[` | If the byte at the data pointer is zero, jump to the matching `]` |
| `]` | If the byte at the data pointer is non-zero, jump back to the matching `[` |

All other characters in a Brainfuck program are treated as comments and ignored.

## Basic Usage

To run a Brainfuck program:

```
brainfuck.exe yourfile.bf
```

The interpreter will execute your program with the default settings.

## Advanced Options

For more control over how your Brainfuck programs run, the following command-line options are available:

| Option | Description | Default |
|--------|-------------|---------|
| `-w` | Enable memory wrapping. When enabled, moving past the end of memory will wrap around to the beginning and vice versa. | Disabled |
| `-d` | Enable debug mode. Prints detailed information about the interpreter state during execution. | Disabled |
| `-m <size>` | Set memory size (number of cells). Use this for programs that need more memory. | 30000 cells |
| `-z` | Set cell to 0 on EOF when using the `,` command. Otherwise, the cell value remains unchanged. | Disabled |

### Examples

Run a program with memory wrapping enabled:
```
brainfuck.exe -w helloworld.bf
```

Run a program with debug information and a larger memory size:
```
brainfuck.exe -d -m 100000 complex_program.bf
```

Run a program with memory wrapping and EOF behavior set to zero:
```
brainfuck.exe -w -z input_heavy.bf
```

## Error Handling

The interpreter provides detailed error messages for common issues:

- Unmatched brackets (`[` or `]`)
- Memory pointer out of bounds (unless wrapping is enabled)
- Too many nested loops
- Memory allocation failures

## Memory Model

The interpreter uses a tape-based memory model with a configurable number of cells (default: 30000). Each cell is an unsigned byte (0-255) that wraps around when incremented past 255 or decremented below 0.

## Input and Output

- Output (`.` command) is displayed directly to the console
- Input (`,` command) is read from the console. In interactive mode, an "Input:" prompt will appear

## Sample Programs

Here are some example Brainfuck programs you can try:

1. **Hello World**
   ```
   ++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
   ```

2. **Cat Program (copies input to output)**
   ```
   ,[.,]
   ```

3. **Addition (adds two single-digit numbers)**
   ```
   ,>,[<+>-]<.
   ```

## Troubleshooting

- If a program seems to hang, it might be waiting for input (`,` command) or stuck in an infinite loop
- If you see "Data pointer out of bounds" errors, try enabling memory wrapping with `-w`
- For large or complex programs, increase the memory size with `-m`
- Use debug mode (`-d`) to see what's happening inside your program

## About Brainfuck

Brainfuck was created in 1993 by Urban Müller. Despite its minimalist design (or perhaps because of it), it has become one of the most popular esoteric programming languages.

While primarily used for fun and educational purposes, writing programs in Brainfuck can help deepen your understanding of computer science fundamentals like Turing machines and computational theory.
