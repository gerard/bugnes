/* Probably the worst example of an 6502 assembler.  Hopefully, is enough for
 * now and is cleaner than just throwing opcodes here and there.  Opcodes are
 * implemented on a need-basis.
 */

#define __DEMUX16(x)    ((x) & 0xFF), (((x) & 0xFF00) >> 8)

#define LDA_IMM(x)      0xA9, (x)
#define LDA_ZPA(x)      0xA5, (x)
#define LDX_IMM(x)      0xA2, (x)
#define STA_ZPA(x)      0x85, (x)
#define STA_ABS(x)      0x8D, __DEMUX16(x)

#define TXS_IMP()       0x9A
#define PHA_IMP()       0x48
#define PLA_IMP()       0x68

#define ADC_IMM(x)      0x69, (x)
#define SBC_ZPA(x)      0xE5, (x)
#define CMP_ZPA(x)      0xC5, (x)

#define BEQ_REL(x)      0xF0, ((uint8_t)x)
#define BNE_REL(x)      0xD0, ((uint8_t)x)
#define JMP_ABS(x)      0x4C, __DEMUX16(x)
#define JSR_ABS(x)      0x20, __DEMUX16(x)
#define RTS_IMP()       0x60

#define CLD_IMP()       0xD8
#define SEC_IMP()       0x38
