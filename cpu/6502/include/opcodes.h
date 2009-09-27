#ifndef _OPCODES_H_
#define _OPCODES_H_

void opcode_fetch_start(uint16_t addr);
void opcode_fetch();
void opcode_exec();

const char *opcode_get_name();
int opcode_get_addr_mode();
int opcode_get_params(uint8_t *byte1, uint8_t *byte2);

#endif
