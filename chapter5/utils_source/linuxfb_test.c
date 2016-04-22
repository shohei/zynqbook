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
box_fill(uint32_t *i32p, int x0, int y0, int x1, int y1)
{
	int x, y;
	uint32_t base;
	uint32_t addr;
	uint32_t *ip;

	base = (uint32_t)i32p;

	for( y = y0 ; y <= y1; ++y) {
		addr = base + y * 4 * 2048;
		for( x = x0, addr+=(x0 * 4); x <= x1; ++x, addr += 4) {
			ip = (uint32_t *)addr;
			*ip = 0xFF00FF00;
		}
	}
}

//----------------------------------------------------------------
void
capture(uint32_t *addr, uint32_t screensize, char *file_name)
{
	int fds;
	int rv;
	fds = open(file_name, O_RDWR | O_TRUNC | O_CREAT, 0666);

	fprintf(stdout, "capture start:%d\n", fds);
	if ( fds < 0 ) {
		perror("open error:");
		return;
	}

	rv = write(fds, addr, screensize);
	if ( rv != screensize ) {
		perror("write error:");
	}

	close(fds);
}

//----------------------------------------------------------------
void
Usage()
{
	fprintf(stderr, "Usage: linuxfb_test [N | /dev/fbN]\n");
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

	printf("The framebuffer device was opened !\n");

	//----------------------------------------------------------------
	if ( ioctl( fds , FBIOGET_FSCREENINFO , &finfo ) ) {
		fprintf(stderr, "Fixed information not gotton !\n");
		exit(2);
	}

	printf("physical 0x%x\n", (int)finfo.smem_start);
	printf("finfo.smem_len %d\n", finfo.smem_len);
	
	//----------------------------------------------------------------
	if ( ioctl( fds , FBIOGET_VSCREENINFO , &vinfo ) ) {
		fprintf(stderr, "Variable information not gotton !\n");
		exit(3);
	}
	xres = vinfo.xres ;
	yres = vinfo.yres ;
	vbpp = vinfo.bits_per_pixel ;
	line_len = finfo.line_length ;

	printf( "%d(pixel)x%d(line), %dbpp(bits per pixel) line_length:%d\n", xres, yres, vbpp, line_len);

	screensize = xres * yres * vbpp / DIV_BYTE ;
	screensize = line_len * yres;
	printf("screensize %d\n", screensize);

	//----------------------------------------------------------------
	fbptr = (char *)mmap(0, finfo.smem_len,PROT_READ | PROT_WRITE,MAP_SHARED,fds,0);
	if ( (int)fbptr == -1 ) {
		fprintf(stderr, "Don't get framebuffer device to memory !\n");
		exit(4);
	}
	close(fds);
	printf("The framebuffer device was mapped !\n");

	//----------------------------------------------------------------
	printf("vinfo.xoffset %d\n", vinfo.xoffset);
	printf("vinfo.yoffset %d\n", vinfo.yoffset);

	if ( argc == 3 ) {
		char *file_name;
		file_name = argv[2];
		capture((uint32_t *)fbptr, screensize, file_name);
		exit(0);
	}
	box_fill((uint32_t *)fbptr, 100, 100, 300, 300);
	sleep(10);

	i = 0;
	tcolor = 0xFF0000FF;
	tcolor <<= ((no - 1) * 8);
	if (vbpp == 16) {
		int r, g, b;
		
		r = tcolor >> 16;
		g = (tcolor & 0x00FF00) >> 8;
		b = tcolor & 0xFF;

		r &= 0xF8;
		g &= 0xFC;
		b &= 0xF8;

		tcolor16 = (r << 8);
		tcolor16 |= (g << 3);
		tcolor16 |= (b >> 3);
	}

	printf("tcolor %x\n", tcolor);
	no_y = no * 80;

	if ( no ) {
		for ( y = 0 ; y < 480 ; y++ ) {
			cp = fbptr + y * 2048 * (vbpp / 8);
			for ( x = 0 ; x < xres ; x++ ) {
				for( i = 0 ; i < vbpp/8 ; ++i ) {
					*cp++ = 0x00;
				}
			}
		}
	}
	for ( y = no_y ; y < no_y + 80; y++ ) {
		cp = fbptr + y * 2048 * (vbpp / 8);

		for( x = 0; x < xres; x++) {
			if ( vbpp == 8 ) {
				*cp = tcolor;
				*cp = (y / 5 % 6);
				*cp = 2;
				cp++;
				tcolor++;
			} else if ( vbpp == 16 ) {
				*(uint16_t *)cp = tcolor16;
				cp += 2;
			} else {
				/*
				tcolor &= 0xFFFFFF;
				tcolor |= ((256 - ( x % 256)) << 24);
				*/

				*(uint32_t *)cp = tcolor;
				cp += 4;
			}
		}
	}

	sleep(10);
	munmap(fbptr,screensize);

	return 0;
}

