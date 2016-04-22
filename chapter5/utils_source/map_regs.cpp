#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

//----------------------------------------------------------------
void *
map_regs(uint32_t regs_phy_addr, uint32_t size)
{
	int fds;
	void *regs;

	fds = open("/dev/mem", O_RDWR);
	if ( fds < 0 ) {
		return 0;
	}
	regs = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fds, regs_phy_addr);
	close(fds);

	return regs;
}
