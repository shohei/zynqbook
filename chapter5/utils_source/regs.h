#ifndef __REGS_H__
#define __REGS_H__

#include <stdint.h>

inline
void
REG_WRITE(void *_reg_base, int offset, uint32_t value) 
{
	uint32_t reg_base_int;
	reg_base_int = reinterpret_cast<uint32_t>(_reg_base) + offset;;
	volatile uint32_t *reg_base;

	reg_base = reinterpret_cast<uint32_t *>(reg_base_int);
	*reg_base = value;
}

inline
uint32_t
REG_READ(void *_reg_base, int offset) 
{
	uint32_t reg_base_int;
	reg_base_int = reinterpret_cast<uint32_t>(_reg_base) + offset;;
	volatile uint32_t *reg_base;

	//printf("read 0x%x\n", reg_base_int);
	reg_base = reinterpret_cast<uint32_t *>(reg_base_int);
	return *reg_base;
}

void *map_regs(uint32_t regs_phy_addr, uint32_t size);

#define LOGICVC_REGS_BASE_ADDR 0x40030000
#define LOGIBITBLT_REGS_BASE_ADDR 0x40080000
#define LOGIBMP_REGS_BASE_ADDR 0x40090000
#define REGS_SIZE 0x10000

#endif /* __REGS_H__ */
