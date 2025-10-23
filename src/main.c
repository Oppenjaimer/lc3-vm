#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/*                                   MEMORY                                   */
/* -------------------------------------------------------------------------- */

#define MEMORY_MAX (1 << 16)
uint16_t mem[MEMORY_MAX];

uint16_t mem_read(uint16_t reg) {
    (void)reg;
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                                  REGISTERS                                 */
/* -------------------------------------------------------------------------- */

#define PC_START 0x3000 // Default PC starting position

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

void update_flags(uint16_t r) {
    if (reg[r] == 0) reg[R_COND] = FL_ZRO;
    else if (reg[r] >> 15) reg[R_COND] = FL_NEG;
    else reg[R_COND] = FL_POS;
}

/* -------------------------------------------------------------------------- */
/*                                INSTRUCTIONS                                */
/* -------------------------------------------------------------------------- */

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
/*                                    UTILS                                   */
/* -------------------------------------------------------------------------- */

bool read_image(const char *path) {
    (void)path;
    return false;
}

uint16_t sign_extend(uint16_t x, int bit_count) {
    // Extend with 1's if negative, else with 0's
    if ((x >> (bit_count - 1)) & 1)
        x |= (0xFFFF << bit_count);

    return x;
}

/* -------------------------------------------------------------------------- */
/*                                    MAIN                                    */
/* -------------------------------------------------------------------------- */

int main(int argc, const char **argv) {
    // Check number of args
    if (argc < 2) {
        fprintf(stderr, "Usage: %s IMAGE_FILE [IMAGE_FILE]...\n", argv[0]);
        exit(2);
    }

    // Read all image files
    for (int i = 1; i < argc; i++) {
        if (!read_image(argv[i])) {
            fprintf(stderr, "Failed to read image file '%s'\n", argv[i]);
            exit(1);
        }
    }

    // Set registers on startup
    reg[R_COND] = FL_ZRO;
    reg[R_PC] = PC_START;

    // Execution loop
    int running = true;
    while (running) {
        // Fetch instruction
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t opcode = instr >> 12;

        switch (opcode) {
            case OP_BR:
                break;
            
            case OP_ADD:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr1 = (instr >> 6) & 0x7;
                bool imm_flag = (instr >> 5) & 0x1;

                if (imm_flag) {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[dr] = reg[sr1] + imm5;
                } else {
                    uint16_t sr2 = instr & 0x7;
                    reg[dr] = reg[sr1] + reg[sr2];
                }

                update_flags(dr);
                break;

            case OP_LD:
                break;

            case OP_ST:
                break;

            case OP_JSR:
                break;

            case OP_AND:
                break;

            case OP_LDR:
                break;

            case OP_STR:
                break;

            case OP_NOT:
                break;

            case OP_LDI:
                break;

            case OP_STI:
                break;

            case OP_JMP:
                break;

            case OP_LEA:
                break;

            case OP_TRAP:
                break;

            case OP_RES:
            case OP_RTI:
            default:
                break;
        }
    }

    return 0;
}