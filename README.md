# 🧠 Advanced LR Parser in C

## Overview

This project implements a **fully functional LR parser** (Look-Ahead Left-to-Right parser) written entirely in C.  
The parser performs **syntactic analysis** for arithmetic expressions using a **table-driven bottom-up parsing engine**.

The implementation emphasizes **dynamic memory management**, **function pointer-based modularity**, and **robust error handling** while maintaining readability and extensibility for compiler or interpreter development.

---

## 🚀 Key Features

### 1. Memory Management and Safety
- **Triple-pointer allocation (`char ***symbols`)** for a 3D parsing table of dimensions `12 × 9 × 4`
- **Dynamic allocation** for grammar components (`stack`, `left`, `right`)
- **Comprehensive allocation checks** with descriptive error messages
- **Automatic cleanup** and controlled error exits
- **Volatile global pointers** for file and buffer safety

### 2. File-Driven Grammar Configuration
- Reads grammar productions from **`gramatica_productie.txt`**
- Loads parsing actions from **`tabel_actiuni.txt`**
- Flexible grammar loading using configurable table sizes (`ROWS`, `COLUMNS`, `MAX_SYMBOL_LENGTH`)

### 3. Table-Driven Parsing Engine
- Uses **12×9 parsing tables** to perform bottom-up LR parsing
- Supports **deplasare (shift)** and **reducere (reduce)** operations
- Implements a **stack-based automaton** with lookahead management
- Dynamically computes **intersection of stack state and next input token**
- Includes **special case handling** for double-digit stack states

### 4. Modular Design with Function Pointers
The grammar structure is encapsulated in the `elemente_gramatica` struct, which includes both **data** and **methods** via function pointers.

```c
typedef struct {
    char ***symbols;            // Parsing table (ROWS x COLUMNS x MAX_SYMBOL_LENGTH)
    unsigned char init_symbol;  // Initial symbol
    unsigned char termen;       // Terminal symbol
    unsigned char factor;       // Factor symbol
    char *stack;                // Parser stack
    char *left;                 // Left-hand side of productions
    char **right;               // Right-hand side of productions
    void (*initializer_members)(...);   // Function pointer for initialization
    handleError_t (*read_table)(void);  // Function pointer for reading table
} elemente_gramatica;
```

### 5. Strong Error Handling via Enumerations
All major operations return structured **error codes** defined in the `handleError_t` enumeration.

```c
typedef enum {
    HANDLE_SUCCES        = 0xAAAAAA33,
    HANDLE_MALLOC_FAIL   = 0xAAAAAAFF,
    HANDLE_FILE_FAIL     = 0xAAAAAACC,
    HANDLE_GENERAL_ERROR = 0xC0000005
} handleError_t;
```

Each function checks for invalid pointers, allocation failures, and I/O errors before proceeding.

---

## 🏗️ Why This Is a Higher-Level C Project

This parser demonstrates a **higher level of abstraction and system design** than typical C programs.  
It pushes C beyond simple procedural usage into **architecture-oriented, memory-safe, and modular programming**.  

### 🔹 1. Object-Oriented Thinking in C
- The use of **function pointers inside structures** (`elemente_gramatica`) simulates object-oriented methods.  
- Each instance of the grammar struct behaves like an **object with methods**, similar to C++ class design.

### 🔹 2. Multi-Dimensional Dynamic Memory Management
- The project performs **3D dynamic allocation** (`char ***symbols`) and ensures correct deallocation, which requires precise pointer arithmetic and memory discipline.  
- This reflects a **deep understanding of C’s memory model**, stack/heap management, and pointer dereferencing at multiple levels.

### 🔹 3. Function-Level Modularity
- Each major task — initialization, reading tables, processing automaton states — is encapsulated in its own function with explicit validation.  
- The code demonstrates **clean modular decomposition** with well-defined responsibilities.

### 🔹 4. Advanced Control Flow and Error Handling
- Uses **error enums**, `perror`, and custom exit codes instead of simple `return` values.  
- Error propagation mimics **exception-like behavior** in higher-level languages.  
- Ensures no undefined behavior through early exits and validation checks.

### 🔹 5. Table-Driven Parsing Logic
- Instead of hardcoded grammar logic, the parser uses **data-driven design** via text-based grammar and action tables.  
- This abstracts syntax analysis away from code logic — a hallmark of **compiler-level architecture**.

### 🔹 6. Use of Volatile and Scoped Globals
- Global file pointers and buffers are declared `static` or `volatile` for scope restriction and thread safety.  
- This demonstrates awareness of **compiler optimization implications** and **data visibility management**.

In summary, this project exemplifies **“higher-level C”** by merging **low-level memory control** with **high-level architectural design**, achieving modularity, flexibility, and maintainability typically seen only in advanced compiler or systems programming.

---

## ⚙️ Main Components

### `initializer_list_vars()`
Allocates all memory for the 3D parsing table and grammar arrays.  
Opens grammar and action files.  
Initializes all symbol cells as empty strings.

---

### `read_table()`
Populates the **parsing table** from `tabel_actiuni.txt`.  
Each cell is filled and validated, printing the result for debugging and verification.

---

### `read_production()`
Parses **grammar production rules** from `gramatica_productie.txt`, populating the left and right symbol arrays.  
Displays all productions read from file.

---

### `automat_evo()`
Implements the **core LR parsing automaton**:
- Finds intersections between stack top and current input symbol  
- Handles **shift (`d`)** and **reduce (`r`)** operations  
- Performs special-case analysis for multi-digit stack indices  
- Calls `special_case()` for complex stack pattern resolution  
- Prints stack evolution step-by-step

---

### `special_case()`
Handles advanced stack operations, including:
- Multi-digit state extraction (`atoi` from tail of stack)
- Stack rollback for invalid transitions
- Automatic rejection or acceptance based on parsing state
- Final acceptance check for valid `$`-terminated strings

---

## 🧩 Example Input

**Grammar file:** `gramatica_productie.txt`
```
E T F
E -> E + T
E -> T
T -> T * F
T -> F
F -> ( E )
F -> id
```

**Action table file:** `tabel_actiuni.txt`
```
id + * ( ) $ E T F
d5  x  x d4  x  x 1 2 3
...
```

**Input string (example in code):**
```c
char entry_message[50] = "id+id+id+id+id*id $";
```

---

## 🧠 Parsing Process

1. Initializes grammar structures and reads input files  
2. Builds parsing table and production rules  
3. Simulates LR parsing automaton:
   - Reads input from `entry_message`
   - Uses **shift/reduce** transitions from the parsing table
   - Prints stack contents at every step
4. Outputs `acceptare` or `respingere` accordingly

---

## 🧾 Example Console Output
```
Symbols[0][0] = id
Symbols[0][1] = +
...
Production 0: E -> E+T
Production 1: T -> F
Initial Symbol E , Termen T, Factor F
Stack: $ 0 
Stiva: $ 0 id5
Stiva: $ 0 E1
...
acceptare
```

---

## 🧰 Technical Highlights

| Feature | Description |
|----------|--------------|
| **Language** | C (C99 Standard) |
| **Memory** | Fully dynamic (no static arrays except macros) |
| **Parsing** | LR(0)-style deterministic bottom-up |
| **I/O** | File-based grammar and table reading |
| **Safety** | Pointer validation and allocation checks |
| **Debugging** | Step-by-step stack tracing with detailed console logs |

---

## 💡 Future Improvements
- Implement grammar validation before parsing  
- Add command-line interface for file input  
- Support variable-length symbols beyond 4 characters  
- Extend to **SLR(1)** or **LR(1)** parsing with lookahead sets  

---

## 🧑‍💻 Author
**Maior Cristian Alexandru**  
Developed as an advanced C programming project demonstrating compiler construction principles and high-level C system architecture.
