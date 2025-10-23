#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/*                                   MEMORY                                   */
/* -------------------------------------------------------------------------- */

#define MEMORY_MAX (1 << 16)
uint16_t mem[MEMORY_MAX];

uint16_t mem_read(uint16_t r) {
    (void)r;
    return 0;
}

void mem_write(uint16_t addr, uint16_t val) {
    (void)addr; (void)val;
}

/* -------------------------------------------------------------------------- */
/*                                  REGISTERS                                 */
/* -------------------------------------------------------------------------- */

#define PC_START 0x3000 // Default PC starting position

enum {
    R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7, // General purpose
    R_PC,   // Program counter
    R_COND, // Condition flags
    R_COUNT // Number of registers
};

enum {
    FL_POS = 1 << 0, // (P)ositive
    FL_ZRO = 1 << 1, // (Z)ero
    FL_NEG = 1 << 2  // (N)egative
};

uint16_t reg[R_COUNT];

void update_flags(uint16_t r) {
    if (reg[r] == 0) reg[R_COND] = FL_ZRO;
    else if (reg[r] >> 15) reg[R_COND] = FL_NEG;
    else reg[R_COND] = FL_POS;
}

/* -------------------------------------------------------------------------- */
/*                                INSTRUCTIONS                                */
/* -------------------------------------------------------------------------- */

enum {
    TRAP_GETC   = 0x20, // Read char from keyboard
    TRAP_OUT    = 0x21, // Write char to console
    TRAP_PUTS   = 0x22, // Write null-terminated string to console
    TRAP_IN     = 0x23, // Read and echo one char from keyboard
    TRAP_PUTSP  = 0x24, // Write null-terminated string of byte pairs to console
    TRAP_HALT   = 0x25  // Halt program execution
};

enum {
    OP_BR,  // Branch
    OP_ADD, // Add
    OP_LD,  // Load
    OP_ST,  // Store
    OP_JSR, // Jump to subroutine
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
};

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
                uint16_t cond_flag = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                if (cond_flag & reg[R_COND])
                    reg[R_PC] += pc_offset;

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
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[dr] = mem_read(reg[R_PC] + pc_offset);
                update_flags(dr);
                break;

            case OP_ST:
                uint16_t sr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                mem_write(reg[R_PC] + pc_offset, reg[sr]);
                break;

            case OP_JSR:
                uint16_t rel_flag = (instr >> 11) & 0x1;
                reg[R_R7] = reg[R_PC];

                if (rel_flag) {
                    uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
                    reg[R_PC] += pc_offset;
                } else {
                    uint16_t base_r = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[base_r];
                }

                break;

            case OP_AND:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr1 = (instr >> 6) & 0x7;
                bool imm_flag = (instr >> 5) & 0x1;

                if (imm_flag) {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[dr] = reg[sr1] & imm5;
                } else {
                    uint16_t sr2 = instr & 0x7;
                    reg[dr] = reg[sr1] & reg[sr2];
                }

                update_flags(dr);
                break;

            case OP_LDR:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t base_r = (instr >> 6) & 0x7;
                uint16_t offset = sign_extend(instr & 0x3F, 6);

                reg[dr] = mem_read(reg[base_r] + offset);
                update_flags(dr);
                break;

            case OP_STR:
                uint16_t sr = (instr >> 9) & 0x7;
                uint16_t base_r = (instr >> 6) & 0x7;
                uint16_t offset = sign_extend(instr & 0x3F, 6);

                mem_write(reg[base_r] + offset, reg[sr]);
                break;

            case OP_NOT:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t sr = (instr >> 6) & 0x7;

                reg[dr] = ~reg[sr];
                update_flags(dr);
                break;

            case OP_LDI:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[dr] = mem_read(mem_read(reg[R_PC] + pc_offset));
                update_flags(dr);
                break;

            case OP_STI:
                uint16_t sr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                mem_write(mem_read(reg[R_PC] + pc_offset), reg[sr]);
                break;

            case OP_JMP:
                uint16_t base_r = (instr >> 6) & 0x7;
                reg[R_PC] = reg[base_r];
                break;

            case OP_LEA:
                uint16_t dr = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[dr] = reg[R_PC] + pc_offset;
                update_flags(dr);
                break;

            case OP_TRAP:
                reg[R_R7] = reg[R_PC];
                
                switch (instr & 0xFF) {
                    case TRAP_GETC:
                        reg[R_R0] = (uint16_t)getchar();
                        update_flags(R_R0);
                        break;
                    
                    case TRAP_OUT:
                        putchar((char)reg[R_R0]);
                        fflush(stdout);
                        break;

                    case TRAP_PUTS:
                        uint16_t *c = mem + reg[R_R0];
                        while (*c) {
                            putchar((char)*c);
                            c++;
                        }

                        fflush(stdout);
                        break;
                    
                    case TRAP_IN:
                        printf("IN: ");
                        char c = getchar();
                        putchar(c);
                        fflush(stdout);

                        reg[R_R0] = (uint16_t)c;
                        update_flags(R_R0);
                        break;

                    case TRAP_PUTSP:
                        uint16_t *c = mem + reg[R_R0];
                        while (*c) {
                            char c1 = (*c) & 0xFF;
                            putchar(c1);

                            char c2 = (*c) >> 8;
                            if (c2) putchar(c2);

                            c++;
                        }

                        fflush(stdout);
                        break;

                    case TRAP_HALT:
                        printf("HALT\n");
                        fflush(stdout);
                        running = 0;
                        break;

                    default:
                        abort();
                        break;
                }

                break;

            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;
        }
    }

    return 0;
}