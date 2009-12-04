#include <stdint.h>
#include <assert.h>

/* These are detailed descriptions of the PPU registers */
typedef enum {
    PPU_REG_INVALID = -1,
    PPU_REG_CR1,
    PPU_REG_CR2,
    PPU_REG_SR,
    PPU_REG_MAX
} ppu_reg_t;

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
uint8_t ppu_reg_i_get(ppu_reg_t reg);
uint8_t ppu_reg_i_get_field(ppu_reg_t reg, uint8_t shift, uint8_t size);
void ppu_reg_i_set(ppu_reg_t reg, uint8_t v);
void ppu_reg_i_set_field(ppu_reg_t reg, uint8_t shift, uint8_t size, uint8_t val);


/* We try not to make the macros too smart and be clear with what the do.  Not
 * that is easy with this nasty preprocessor tricks, but I try :P
 * *shift* is set to a value of one of the _SHIFT enums.  *mask* is the mask
 * that derives from _SHIFT and _SIZE.  After the macro magic is done (conca-
 * tenation), we give it to a internal function to do the job.
 */
#define PPU_REG_GETF(reg, field) (({                                    \
    ppu_reg_t ppu_reg = PPU_REG_ ## reg;                                \
    uint8_t shift = reg ## _ ## field ## _SHIFT;                        \
    uint8_t size  = reg ## _ ## field ## _SIZE;                         \
                                                                        \
    ppu_reg_i_get_field(ppu_reg, shift, size);                          \
}))

#define PPU_REG_SETF(reg, field, value) (({                             \
    ppu_reg_t ppu_reg = PPU_REG_ ## reg;                                \
    uint8_t shift = reg ## _ ## field ## _SHIFT;                        \
    uint8_t size  = reg ## _ ## field ## _SIZE;                         \
                                                                        \
    ppu_reg_i_set_field(ppu_reg, shift, size, value);                   \
}))

/* Setters/Getters for the whole register */
#define PPU_REG_GET(reg)        ppu_reg_i_get(PPU_REG_ ## reg)
#define PPU_REG_SET(reg, v)     ppu_reg_i_set(PPU_REG_ ## reg, v);
