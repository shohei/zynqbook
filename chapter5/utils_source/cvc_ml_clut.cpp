#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "regs.h"

#define DEVICE_NAME "/dev/fb0"
#define DIV_BYTE 8
char device_name[] = DEVICE_NAME;

#define X_PIXEL_MAX 2048
#define Y_LINE_MAX  2048

static int no;

void
cvc_ml_clut(void *cvc_regs)
{
	int clut_top, layer_no, clut_no;
	int clut_sel;
	uint32_t ip_version;
	int index_no;

	ip_version = REG_READ(cvc_regs, 0xF8);
	printf("ip_version:0x%x\n", ip_version);

	/*
	for( int i = 0x1000 ; i < 0x6000; i += 4) {
		REG_WRITE(cvc_regs, i, 0xFFFFFFFF);
	}
	REG_WRITE(cvc_regs, 0x138, 0);
	REG_WRITE(cvc_regs, 0x1B8, 0);
	REG_WRITE(cvc_regs, 0x238, 0);
	REG_WRITE(cvc_regs, 0x2b8, 0);
	REG_WRITE(cvc_regs, 0x338, 0);
	*/
	REG_WRITE(cvc_regs, 0x2b8, 1);

	layer_no = 3;
	clut_no = 0;
	clut_top = 0x1000 + 0x1000 * layer_no + 0x800 * clut_no;

	printf("clut_top: 0x%x\n", clut_top);

	index_no = 0;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0xFFFF0000);
	index_no = 1;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0xFF00FF00);
	index_no = 2;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0xFF0000FF);

	index_no = 3;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0x7FFF0000);
	index_no = 4;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0x7F00FF00);
	index_no = 5;
	REG_WRITE(cvc_regs, clut_top + index_no * 8 + 4, 0x7F0000FF);
	/*
	for( int i = clut_top; i < clut_top + 0x800 ; i += 4 ) {
		REG_WRITE(cvc_regs, i, 0xFFFF00FF);
	}
	*/

	clut_sel = ((clut_no) << (layer_no * 2)) | ( 1 << ( 10 + layer_no));
	printf("clut_sel: 0x%x\n", clut_sel);
	REG_WRITE(cvc_regs, 0x60, clut_sel);
	sleep(1);

	clut_sel = REG_READ(cvc_regs, 0x60);
	printf("after clut_sel: 0x%x\n", clut_sel);
}


//----------------------------------------------------------------

int main(int argc, char **argv)
{
	int fds ;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	int screensize ;
	char *fbptr;

	int xres, yres, vbpp, line_len;

	//----------------------------------------------------------------
	if ( argc == 2 ) {
		device_name[7] = argv[1][0];
		no = argv[1][0] - 0x30;
	}

	fds = open(device_name, O_RDWR);

	if ( fds < 0 ) {
		fprintf(stderr, "Framebuffer(%s) device open error !\n", device_name);
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
	printf("screensize %d\n", screensize);

	//----------------------------------------------------------------
	fbptr = (char *)mmap(0,finfo.smem_len,PROT_READ | PROT_WRITE,MAP_SHARED,fds,0);
	if ( fbptr == MAP_FAILED ) {
		fprintf(stderr, "Don't get framebuffer device to memory !\n");
		exit(4);
	}
	printf("The framebuffer device was mapped !\n");

	// you can close after mmap
	close(fds);
	fds = -1;

	for( int i = 0 ; i < 16 * 16; ++i ) {
		*(fbptr + i) = 0xFF - i;
	}

	//cvc regs 0x40030000 - 0x4003ffff
	void *cvc_regs;
	cvc_regs = map_regs(0x40030000, 0x10000);
	if ( cvc_regs == 0 ) {
		fprintf(stderr, "map_regs error %x\n", 0x40030000);
		goto err;
	}
	fprintf(stderr, "map_addr:0x%x\n", (uint32_t)cvc_regs);
	cvc_ml_clut(cvc_regs);

	getchar();
	munmap(cvc_regs, 0x10000);
err:
	munmap(fbptr, screensize);

	return 0;
}
