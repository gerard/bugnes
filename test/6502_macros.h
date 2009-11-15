/* Probably the worst example of an 6502 assembler.  Hopefully, is enough for
 * now and is cleaner than just throwing opcodes here and there.  Opcodes are
 * implemented on a need-basis.
 */

#define LDA_IMM(x)      0xA9, (x)
#define LDA_ZPA(x)      0xA5, (x)

#define STA_ZPA(x)      0x85, (x)

#define JMP_ABS(x)      0x4C, ((x) & 0xFF), (((x) & 0xFF00) >> 8)
