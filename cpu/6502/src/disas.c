#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "cpu/sfot.h"

/* This is just awful, but hopefully manageable */

void disassemble_opcode(struct sfot_step_info *info)
{
    memset(info->opcode_decoded, ' ', 24);
    sprintf(info->opcode_decoded, "%04X: %s", info->reg_pc, info->opcode_name);

    char *buf = &info->opcode_decoded[9];
    uint8_t byte1 = info->opcode_par[0];
    uint8_t byte2 = info->opcode_par[1];
    int i;

    if (info->opcode_par_n == 0) {
        switch (info->addr_mode) {
        case SFOT_A_ACC:
            sprintf(buf, " A");
            break;
        case SFOT_A_IMP:
            break;
        default:
            assert(0);
        }
        goto proc_next;
    }

    if (info->opcode_par_n == 1) {
        switch (info->addr_mode) {
        case SFOT_A_IMM:
            sprintf(buf, " #%02X", byte1);
            break;
        case SFOT_A_ZPA:
            sprintf(buf, " $%02X", byte1);
            break;
        case SFOT_A_ZPX:
            sprintf(buf, " $%02X,X", byte1);
            break;
        case SFOT_A_ZPY:
            sprintf(buf, " $%02X,Y", byte1);
            break;
        case SFOT_A_REL:
            sprintf(buf, " *%+d", (int8_t)byte1);
            break;
        case SFOT_A_INX:
            sprintf(buf, " ($%02X,X)", byte1);
            break;
        case SFOT_A_INY:
            sprintf(buf, " ($%02X),Y", byte1);
            break;
        default:
            assert(0);
        }
        goto proc_next;
    }
    
    switch (info->addr_mode) {
    case SFOT_A_ABS:
        sprintf(buf, " %04X", (byte2 << 8) + byte1);
        break;
    case SFOT_A_ABX:
        sprintf(buf, " %04X,X", (byte2 << 8) + byte1);
        break;
    case SFOT_A_ABY:
        sprintf(buf, " %04X,Y", (byte2 << 8) + byte1);
        break;
    case SFOT_A_IND:
        sprintf(buf, " (%04X)", (byte2 << 8) + byte1);
        break;
    default:
        assert(0);
    }   

proc_next:
    while(*buf) buf++;
    *buf = ' ';
    buf = &info->opcode_decoded[24];
    for (i = 0; i < SFOT_REG_MAX; i++)
    {
        uint8_t reg = info->registers[i];
        buf += sprintf(buf, reg ? " [%02X]" : " [  ]", reg);
    }
    *buf = 0;
}
