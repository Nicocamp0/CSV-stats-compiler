# AFSTAT Compiler

Compiler for **AFSTAT**, a domain-specific language designed to perform statistical analysis on CSV datasets.

The compiler parses AFSTAT programs and generates **C code** that performs the requested analyses.

---

# Features

- Lexical analysis using **Flex**
- Syntax analysis using **Bison**
- Semantic analysis
- Symbol table construction
- Abstract Syntax Tree (AST)
- Code generation in C

---

# Project Structure

```
src/        implementation files
include/    header files
examples/   AFSTAT programs and CSV datasets
tests/      additional test programs
docs/       report and presentation
```

---

# Build

From the project root directory:

```bash
make
```

This generates the executable:

```
analyseur
```

---

# Usage

Generate C code from an AFSTAT program:

```bash
./analyseur -o out.c < examples/prog1.afs
```

Display the symbol table:

```bash
./analyseur -tos < examples/prog1.afs
```

Display tokens extracted by the lexer:

```bash
./analyseur -tokens < examples/prog1.afs
```

Display the Abstract Syntax Tree:

```bash
./analyseur -ast < examples/prog1.afs
```

Display compiler information:

```bash
./analyseur -version
```

---

# Example

Example AFSTAT program:

```
source med "examples/medical_records_full.csv";
```

The compiler generates a C program that performs the requested statistical analysis.

---

# Authors

Project developed as part of a **Master's degree in Computer Science**.

Nicolas Ventadoux  
University of Strasbourg