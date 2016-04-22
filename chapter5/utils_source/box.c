#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/fb0"
#define DIV_BYTE 8
char device_name[] = DEVICE_NAME;

#define X_PIXEL_MAX 2048
#define Y_LINE_MAX  2048

static int no;

void
box_fill(uint32_t *i32p, int x0, int y0, int x1, int y1, int stride)
{
	int x, y;
	uint32_t base;
	uint32_t addr;
	uint32_t *ip;

	base = (uint32_t)i32p;

	for( y = y0 ; y <= y1; ++y) {
		addr = base + y * stride;
		for( x = x0, addr+=(x0 * 4); x <= x1; ++x, addr += 4) {
			ip = (uint32_t *)addr;
			*ip = 0xFF00FF00;
		}
	}
}

//----------------------------------------------------------------
void
Usage()
{
	fprintf(stderr, "Usage: box [N | /dev/fbN]\n");
}

//----------------------------------------------------------------

int main(int argc, char **argv)
{
	int fds ;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	int screensize ;
	char *fbptr, *cp;

	int x, y ;
	int no_y;
	int xres, yres, vbpp, line_len;
	int i;
	uint32_t tcolor;
	uint16_t tcolor16;

	//----------------------------------------------------------------
	if ( argc >= 2 ) {
		if ( argv[1][0] == '/' ) {
			// 01234567
			// /dev/fbN
			device_name[7] = argv[1][7];
			no = argv[1][7] - 0x30;
			if ((no < 0 ) || ( 4 < no )) {
				Usage();
				exit(0);
			}
		} else {
			device_name[7] = argv[1][0];
			no = argv[1][0] - 0x30;
		}
	}

	fds = open(device_name, O_RDWR);

	if ( fds < 0 ) {
		fprintf(stderr, "Framebuffer(%s) device open error !\n", device_name);
		perror("why:");
		exit(1);
	}

	//----------------------------------------------------------------
	if ( ioctl( fds , FBIOGET_FSCREENINFO , &finfo ) ) {
		fprintf(stderr, "Fixed information not gotton !\n");
		exit(2);
	}
	
	//----------------------------------------------------------------
	if ( ioctl( fds , FBIOGET_VSCREENINFO , &vinfo ) ) {
		fprintf(stderr, "Variable information not gotton !\n");
		exit(3);
	}
	xres = vinfo.xres ;
	yres = vinfo.yres ;
	vbpp = vinfo.bits_per_pixel ;
	line_len = finfo.line_length ;

	screensize = xres * yres * vbpp / DIV_BYTE ;
	screensize = line_len * yres;

	//----------------------------------------------------------------
	fbptr = (char *)mmap(0, finfo.smem_len,PROT_READ | PROT_WRITE,MAP_SHARED,fds,0);
	if ( (int)fbptr == -1 ) {
		fprintf(stderr, "Don't get framebuffer device to memory !\n");
		exit(4);
	}
	close(fds);

	//----------------------------------------------------------------

	box_fill((uint32_t *)fbptr, 100, 100, 300, 300, line_len);

	munmap(fbptr,screensize);

	return 0;
}

