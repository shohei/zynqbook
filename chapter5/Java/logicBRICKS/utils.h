#ifndef __UTIL_H__
#define __UTIL_H__

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace {
inline
void *map_dev_mem(off_t offset_off_t, unsigned int size) 
{
	int fds;

	fds = open("/dev/mem", O_SYNC | O_RDWR);
	if ( fds < 0 ) {
		return MAP_FAILED;
	}

	void *addr;
	addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fds, offset_off_t);
	close(fds);
	if ( addr == MAP_FAILED ) {
		return MAP_FAILED;
	}

	return addr;
}

//----------------------------------------------------------------
inline
uint32_t read_reg32(void *reg_addr, uint32_t offset)
{
	off_t reg_addr_off_t(reinterpret_cast<off_t>(reg_addr));
	reg_addr_off_t += offset;
	volatile uint32_t *reg32(reinterpret_cast<volatile uint32_t *>(reg_addr_off_t));
	return *reg32;
}

//----------------------------------------------------------------
inline
void write_reg32(void *reg_addr, uint32_t offset, uint32_t value)
{
	off_t reg_addr_off_t(reinterpret_cast<off_t>(reg_addr));
	reg_addr_off_t += offset;
	volatile uint32_t *reg32(reinterpret_cast<volatile uint32_t *>(reg_addr_off_t));
	*reg32 = value;
}

//----------------------------------------------------------------
inline
void bit_write_reg32(void *reg_addr, uint32_t offset, uint32_t mask_value, uint32_t bit_value)
{
	off_t reg_addr_off_t(reinterpret_cast<off_t>(reg_addr));
	reg_addr_off_t += offset;
	volatile uint32_t *reg32(reinterpret_cast<volatile uint32_t *>(reg_addr_off_t));
	uint32_t value;
	value = *reg32;
	value &= ~mask_value;
	value |= (mask_value & bit_value);
	*reg32 = value;
}

} // namespace
#endif /* __UTIL_H__ */
