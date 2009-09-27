#ifndef _ADDR_H_
#define _ADDR_H_

typedef enum {
    A_IMP,
    A_ACC,
    A_IMM,
    A_ZPA,
    A_ZPX,
    A_ZPY,
    A_REL,
    A_ABS,
    A_ABX,
    A_ABY,
    A_IND,
    A_INX,
    A_INY,
    A_MAX
} addr_mode_t;

#define INVALID_ADDRESS         ((uint16_t)(0xFFFF))
uint8_t VALUE(addr_mode_t);
uint16_t ADDR(addr_mode_t);
int ADDR_VALID(uint16_t);
uint8_t ADDR_MODE_GET_VALUE(addr_mode_t);

#endif
