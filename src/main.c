#include <stdint.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                                 DEFINITIONS                                */
/* -------------------------------------------------------------------------- */

/* --------------------------------- Memory --------------------------------- */

#define MEMORY_MAX (1 << 16)
uint16_t mem[MEMORY_MAX];

/* -------------------------------- Registers ------------------------------- */

typedef enum {
    R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7, // General purpose
    R_PC,   // Program counter
    R_COND, // Condition flags
    R_COUNT // Number of registers
} Register;

typedef enum {
    FL_POS = 1 << 0, // (P)ositive
    FL_ZRO = 1 << 1, // (Z)ero
    FL_NEG = 1 << 2  // (N)egative
} ConditionFlag;

uint16_t reg[R_COUNT];

/* ------------------------------ Instructions ------------------------------ */

typedef enum {
    OP_BR,  // Branch
    OP_ADD, // Add
    OP_LD,  // Load
    OP_ST,  // Store
    OP_JSR, // Jump register
    OP_AND, // Bitwise AND
    OP_LDR, // Load register
    OP_STR, // Store register
    OP_RTI, // Return from interrupt (unused)
    OP_NOT, // Bitwise NOT
    OP_LDI, // Load indirect
    OP_STI, // Store indirect
    OP_JMP, // Jump
    OP_RES, // Reserved (unused)
    OP_LEA, // Load effective address
    OP_TRAP // Execute trap
} Opcode;

/* -------------------------------------------------------------------------- */
/*                                  FUNCTIONS                                 */
/* -------------------------------------------------------------------------- */

int main() {
    printf("Hello, World!\n");

    return 0;
}