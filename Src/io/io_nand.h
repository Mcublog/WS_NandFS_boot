#ifndef IO_NAND_H
#define IO_NAND_H

#include <stdint.h>

typedef struct
{
    uint32_t page_size;     // Page size
    uint32_t block_number;  // TOTAL Number of block in plane 
    uint32_t block_size;    // Block size (In Page)
    uint32_t plane_number;  // Number of plane
    uint32_t plane_size;    // Plane size (In Page)
}io_nand_cfg_t;

uint32_t io_nand_init(void);
uint32_t io_nand_init_cfg(void);
uint32_t io_nand_set_cfg(uint32_t p_size, uint32_t b_num, uint32_t b_size, uint32_t pl_num);
void     io_nand_get_cfg(io_nand_cfg_t *cfg);

uint32_t io_nand_get_page_size(void);
uint32_t io_nand_get_block_number(void);
uint32_t io_nand_get_block_size(void);
uint32_t io_nand_get_plane_number(void);
uint32_t io_nand_get_plane_size(void);

uint32_t io_nand_read_8b (uint32_t addr, uint8_t *buffer, uint32_t size, uint32_t offst);
uint32_t io_nand_write_8b(uint32_t addr, uint8_t *buffer, uint32_t size, uint32_t offst);
void     io_nand_erase(uint32_t addr);

#endif // IO_NAND_H
