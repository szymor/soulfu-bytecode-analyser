# SoulFu Bytecode Analyser

A command-line tool for analyzing **SoulFu** `.RUN` files, which are compiled bytecode from `.SRC` scripts used for in-game logic in the SoulFu game engine.

## Overview

The SoulFu Bytecode Analyser disassembles and displays the contents of compiled SoulFu script files (.RUN). It provides:
- Fast function table with entry points
- Function definitions with signatures and addresses
- String table contents
- Full bytecode disassembly with opcode mnemonics and arguments
- Human-readable labels for functions in the disassembly

## Features

- **Fast Function Analysis**: Lists 16 predefined fast functions (like `Spawn()`, `Refresh()`, `Event()`) and their entry points
- **Function Table**: Displays all user-defined functions with their return types, names, parameters, and code addresses
- **String Table**: Shows all embedded string constants used by the script
- **Bytecode Disassembly**: Provides a full instruction-by-instruction breakdown of the bytecode with:
  - Address offsets in hexadecimal
  - Raw byte values
  - Opcode mnemonics (e.g., `ADD`, `CALLFUNCTION`, `JUMP`)
  - Argument decoding (variables, constants, jump targets)
- **Label Integration**: Functions are labeled in the disassembly output for easier navigation

## Building

The analyser is written in standard C and requires only a C compiler.

### Using GCC:
```bash
gcc -o soulfu_analyser main.c
```

### Using the provided Makefile:
```bash
make
```

## Usage

```bash
./soulfu_analyser <input.run>
```

### Example Output:
```
SoulFu Byte Code Analyser

File PSPLASH.RUN, loaded 1449 bytes.
Fast functions:
  0100 Refresh()
  0249 Event()
5 function(s):
  0100 int REFRESH()
  0249 int EVENT()
  0297 int SUBSPLASH(float, float, float, int, float, float, int, float, int)
  0369 int ROWSPLASHEFFECT(float, float, float, float, float)
  0411 int EFFECT(float, float, float)
7 string(s):
  FILE:LITESPLI.RGB
  FILE:LAVASPLI.RGB
  FILE:LITESPLI.RGB
  FILE:LAVASPLI.RGB
  FILE:BAD1.RAW
  FILE:LAVASPLI.RGB
  FILE:LITESPLI.RGB

REFRESH:
0100:  1a          FINDSELF
0101:  0080        EQUALS I00
0103:  a0a07b      PUSH [I00].123
0105:  1e          TOFLOAT
0106:  e63dcccccd  PUSH FLOAT 0.100000
010b:  23          F_MULTIPLY
010c:  20c1        F_EQUALS F01
010e:  a0a080      PUSH [I00].128
0110:  e400f0      PUSH SHORT 00f0
0113:  07          ISEQUAL
0114:  430172      IFFALSEJUMP 0172
0117:  a0a000      PUSH [I00].0
0119:  a0a001      PUSH [I00].1
011b:  40..        CALLFUNCTION 00000d49 0000013b 000001d1 00000083 "ROOMWATERLEVEL"
013b:  e2          PUSH 1.0f
013c:  21          F_ADD
013d:  e309        PUSH BYTE 09
...
```

## Bytecode Format

SoulFu `.RUN` files follow this structure:

### Header
- **Fast Function Offsets**: 16 entries × 2 bytes each (big-endian)
- **Function Count**: 2 bytes
- **String Count**: 2 bytes

### Function Table
For each function:
- Address (2 bytes, big-endian)
- Function name (null-terminated string)
- Return type character ('I' for int, 'F' for float) followed by parameter types (null-terminated)

### String Table
For each string:
- Two dummy bytes (0xFF 0xFF)
- String content (null-terminated)

### Bytecode Section
The actual compiled instructions follow the tables. Opcodes are single bytes (0-127) with various encoding schemes for arguments:
- **Variable access**: Bits indicate variable type (int/float/property) and index
- **Immediate values**: Extended opcodes (224-255) with different sizes (byte, short, int, float)
- **Control flow**: Jump addresses (2 bytes, big-endian)
- **Function calls**: Complex 16-byte structure plus function name

## Opcode Reference

The analyser recognizes 128 opcodes covering:
- **Arithmetic operations** (ADD, SUBTRACT, MULTIPLY, DIVIDE)
- **Comparison operations** (ISEQUAL, ISGREATER, etc.)
- **Logical operations** (LOGICALAND, LOGICALOR, LOGICALNOT)
- **Type conversions** (TOFLOAT, TOINT)
- **String operations** (STRINGAPPEND, STRINGCOMPARE, STRINGLENGTH)
- **Control flow** (JUMP, IFFALSEJUMP, CALLFUNCTION, RETURNINT)
- **Game engine functions** (SPAWN, PLAYSOUND, ROLLDICE, DAMAGETARGET)
- **Window system operations** (WINDOWBORDER, WINDOWSTRING, WINDOW3D...)

## Limitations

- The analyser is read-only and cannot modify `.RUN` files
- Some opcodes may be marked as `ERRxx` where the exact semantics are unknown
- Property access decoding is basic (shows property numbers but not symbolic names)
- No verification of bytecode validity

## Related Tools

This analyser complements the SoulFu game engine and its script compiler. For more information about SoulFu game development, refer to the official documentation.

## License

This tool is provided for educational and analysis purposes. SoulFu and its file formats are property of their respective owners.
