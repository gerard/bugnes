#include <stdint.h>
#include <assert.h>

/* These are detailed descriptions of the PPU registers */

/* SIZES: All these are in bits.  Since all the registers are 8 bits, all SIZE
 * enums have to sum up 8 and nothing else
 */
enum {
    CR1_NAME_TABLE_ADDR_SIZE            = 2,
    CR1_VERTICAL_WRITE_SIZE             = 1,
    CR1_SPRITE_PATT_TABLE_SIZE          = 1,
    CR1_SCREEN_PATT_TABLE_SIZE          = 1,
    CR1_SPRITE_SIZE_SIZE                = 1,
    CR1_PPU_MASTER_SLAVE_SIZE           = 1,        /* Not used in NES */
    CR1_VBLANK_ENABLE_SIZE              = 1,
};

enum {
    CR2_BYTE0_UNKNOWN_SIZE              = 1,
    CR2_IMAGE_MASK_SIZE                 = 1,
    CR2_SPRITE_MASK_SIZE                = 1,
    CR2_SCREEN_ENABLE_SIZE              = 1,
    CR2_SPRITES_ENABLE_SIZE             = 1,
    CR2_BACKGROUND_COLOR_SIZE           = 3,
};

enum {
    SR_BYTES05_UNKNOWN_SIZE             = 6,
    SR_HIT_FLAG_SIZE                    = 1,
    SR_VBLANK_FLAG_SIZE                 = 1,
};


/* SHIFTS: Necessary shift to reach the position of the field. */
enum {
    CR1_NAME_TABLE_ADDR_SHIFT           = 0,
    CR1_VERTICAL_WRITE_SHIFT            = 2,
    CR1_SPRITE_PATT_TABLE_SHIFT         = 3,
    CR1_SCREEN_PATT_TABLE_SHIFT         = 4,
    CR1_SPRITE_SIZE_SHIFT               = 5,
    CR1_PPU_MASTER_SLAVE_SHIFT          = 6,        /* Not used in NES */
    CR1_VBLANK_ENABLE_SHIFT             = 7,
};

enum {
    CR2_BYTE0_UNKNOWN_SHIFT             = 0,
    CR2_IMAGE_MASK_SHIFT                = 1,
    CR2_SPRITE_MASK_SHIFT               = 2,
    CR2_SCREEN_ENABLE_SHIFT             = 3,
    CR2_SPRITES_ENABLE_SHIFT            = 4,
    CR2_BACKGROUND_COLOR_SHIFT          = 5,
};

enum {
    SR_BYTES05_UNKNOWN_SHIFT            = 0,
    SR_HIT_FLAG_SHIFT                   = 6,
    SR_VBLANK_FLAG_SHIFT                = 7,
};

/* SETTERS/GETTERS: Nasty preprocessor to avoid writing one function for every
 * field/register
 */

/* These are just helpers for the macros defined in ppu_registers.h.  They are
 * not supposed to be called directly ever.
 */
uint8_t ppu_reg_i_CR1_get();
uint8_t ppu_reg_i_CR2_get();
uint8_t ppu_reg_i_SR_get();

void    ppu_reg_i_CR1_set(uint8_t v);
void    ppu_reg_i_CR2_set(uint8_t v);
void    ppu_reg_i_SR_set(uint8_t v);

/* We try not to make the macros too smart and be clear with what the do.  Not
 * that is easy with this nasty preprocessor tricks, but I try :P
 * *shift* is set to a value of one of the _SHIFT enums.  *mask* is the mask
 * that derives from _SHIFT and _SIZE. *clearval* is the register with the
 * field being modified cleared, while *newval* is the register with the new
 * value in.
 */
#define PPU_REG_GETF(reg, field) (({                                    \
    uint8_t shift = reg ## _ ## field ## _SHIFT;                        \
    uint8_t mask = ((1 << reg ## _ ## field ## _SIZE) - 1) << shift;    \
                                                                        \
    (ppu_reg_i_ ## reg ## _get() << shift) & mask >> shift;             \
}))

#define PPU_REG_SETF(reg, field, value) (({                             \
    uint8_t shift = reg ## _ ## field ## _SHIFT;                        \
    uint8_t mask = ((1 << reg ## _ ## field ## _SIZE) - 1) << shift;    \
    uint8_t clearval = ppu_reg_i_ ## reg ## _get() & ~mask;             \
    uint8_t newval = clearval | (value << shift);                       \
                                                                        \
    assert((value << shift) <= mask);                                   \
    ppu_reg_i_ ## reg ## _set(newval);                                  \
}))

/* Setters/Getters for the whole register */
#define PPU_REG_GET(reg)        ppu_reg_i_ ## reg ## _get()
#define PPU_REG_SET(reg, v)     (({                                     \
    assert(v <= 0xFF);                                                  \
    ppu_reg_i_ ## reg ## _set(v);                                       \
}))
