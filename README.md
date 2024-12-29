# SILCompiler

## Simple Imperative Language Compiler

SILCompiler is a simple, iterative language compiler designed to process a basic imperative programming language with constructs such as assignments, conditionals, loops, procedures, and basic arithmetic operations. This project is built using Lex and Yacc (Flex and Bison) tools to tokenize and parse the source code and generate meaningful results.

### Features:
- **Arithmetic operations**: Addition, subtraction, multiplication, division, and modulus.
- **Conditionals**: `if`, `else`, and `endif`.
- **Loops**: `while`, `repeat until`, and `for`.
- **Procedures**: Support for defining and calling procedures with parameters.
- **Declarations**: Declare variables, including arrays.

### Structure

The compiler consists of two main components:
1. **Lexer**: Written in Flex (lexer.l) - Tokenizes the input code into a sequence of tokens.
2. **Parser**: Written in Bison (parser.y) - Defines the syntax rules and parses the token sequence to construct an Abstract Syntax Tree (AST).

### Lexer (`lexer.l`)
The lexer file defines the regular expressions for various tokens in the language. Some of the key tokens include:
- Keywords: `PROCEDURE`, `IS`, `PROGRAM`, `IF`, `THEN`, `ELSE`, `WHILE`, `DO`, etc.
- Arithmetic operators: `+`, `-`, `*`, `/`, `%`.
- Relational operators: `=`, `!=`, `>`, `<`, `>=`, `<=`.
- Special symbols: `:`, `=`, `;`, `(`, `)`, `[`, `]`, `,`.

### Parser (`parser.y`)
The parser defines the grammar rules for the language, including constructs such as:
- **Program structure**: Programs start with `PROGRAM IS` and end with `END`.
- **Commands**: Includes assignments, conditionals, loops, procedure calls, and I/O operations like `READ` and `WRITE`.
- **Expressions**: Support for simple expressions with operands and operators.
- **Declarations**: Supports variable declarations and array declarations.

### Build Instructions

The project comes with a `Makefile` to simplify the build process. To build the compiler, follow these steps:

1. **Install dependencies**:
   - Install `flex` (for lexing) and `bison` (for parsing).
     - On Ubuntu/Debian: `sudo apt install flex bison`
     - On macOS (using Homebrew): `brew install flex bison`

2. **Build the project**:
   Simply run `make` in the project directory. This will generate the lexer and parser, then compile the C++ files into an executable.

   ```bash
   make
   ```

   This command will:
   - Run `bison` to generate the parser (`parser.cpp` and `parser.hpp`).
   - Run `flex` to generate the lexer (`lexer.cpp`).
   - Compile the `parser.cpp`, `lexer.cpp`, and link them into the final executable (`parse`).

3. **Clean the project**:
   To clean the generated files, run:

   ```bash
   make clean
   ```

   This will remove the generated source files and the executable.

4. **Run the compiler**:
   After building the project, run the compiler on your source file:

   ```bash
   ./parse input_file.imp
   ```

   Replace `input_file.imp` with your source code in the Simple Imperative Language format.

### Example

Here’s an example of a simple program in SIL (Simple Imperative Language):

```sil
PROGRAM IS
   PROCEDURE sum IS
      DECLARE x, y
      x := 10;
      y := 20;
      WRITE x + y;
   END
   BEGIN
      sum();
   END
```

Other example programs and whole language specification can be found in [**materiały**](https://github.com/kasprzakewa/SILCompiler/tree/main/materia%C5%82y) directory.

### License

This project is licensed under the [**Apache License 2.0**](https://www.apache.org/licenses/LICENSE-2.0).

### Attribution

The language and virtual machine used in this compiler were developed by [**Dr. hab. Maciej Gebala**](https://cs.pwr.edu.pl/gebala/), a tutor at [**Wroclaw University of Science and Technology**](https://pwr.edu.pl/).
