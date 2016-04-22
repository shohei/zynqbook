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

#define CTRL0_REG    0x00
#define CTRL1_REG    0x04
#define ROP_REG      0x08
#define OP_REG       0x0c
#define SRC_ADDR_REG 0x10
#define DST_ADDR_REG 0x14
#define SRC_STRD_REG 0x18
#define DST_STRD_REG 0x1C
#define X_WIDTH_REG  0x20
#define Y_WIDTH_REG  0x24
#define BG_COLOR_REG 0x28
#define FG_COLOR_REG 0x2C
#define IP_VERSION_REG 0x34

#define BITBLT_CTRL0_START_MSK          0x80
#define BITBLT_CTRL0_SRC_LIN_MSK        0x01

//#define BITBLT_MOVE_COL_EXP_CMD             0x0A
#define BITBLT_SOLID_FILL_CMD               0x0C
#define BITBLT_PAT_FILL_ROP_CMD             0x06
#define BITBLT_PAT_FILL_TRANS_CMD           0x07
#define BITBLT_AA_FONT_EXPAND_CMD           0x0D
#define BITBLT_MOVE_ROP_P_CMD               0x02
#define BITBLT_PORTER_DUFF_CMD              0x08

#define BITBLT_ROP_NOTSORNOTD_CMD	0x07
#define BITBLT_ROP_S_CMD                0x0C
#define BITBLT_ROP_WHITE_CMD            0x0F

#define PLUS 9

static int no;

void
bitblt_ip_version(void *bitblt_regs)
{
	uint32_t ip_version;
	ip_version = REG_READ(bitblt_regs, IP_VERSION_REG);
	fprintf(stderr, "ip_version:0x%08x\n", ip_version);
	fprintf(stderr, "ip_version:0x%d:", (ip_version >> 17)& 0x3);
	fprintf(stderr, "0x%d:", (ip_version >> 11)& 0x3F);
	fprintf(stderr, "0x%d:", (ip_version >> 5)& 0x3F);
	fprintf(stderr, "0x%d\n", (ip_version >> 0)& 0x1F);
}

void
view_vram(int top_phy_addr, int x, int y, int w, int h)
{
	void *vram;
	vram = map_regs(top_phy_addr, 2048 * 4 * 2048);
	for( int yi = y; yi < (y + h); ++yi ) {
		for( int xi = x; xi < (x + w); ++xi ) {
			uint32_t v;
			v = REG_READ(vram, yi * 2048 * 4 + xi * 4);
			printf("%08x ", v);
		}
		printf("\n");
	}

	munmap(vram, 0);
}

void
wait_until(void *bitblt_regs)
{
	uint32_t v;
	do {
		v = REG_READ(bitblt_regs, CTRL0_REG);
		printf("CTRL0_REG:0x%08x\n", v);
	} while ( v & 0x80 );
}

void
bitblt_solid(void *bitblt_regs)
{
	uint32_t src_addr, dst_addr;
	uint32_t dst_stride;
	uint32_t bg_color, fg_color;
	uint32_t x_width, y_width;

	src_addr = 0x31950000; //layer3 top address /dev/video0
	dst_addr = 0x30000000 + 500 * 4 + 2048 * 50 * 4; // layer0 top address /dev/video3

	dst_stride = 2048 * 4;
	x_width = 64 * 4 - 1;
	y_width = 64 - 1;
	fg_color = 0xFF0000FF;
	bg_color = 0xFF0000FF;

	REG_WRITE(bitblt_regs, CTRL1_REG, 0x00000002);
	REG_WRITE(bitblt_regs, DST_ADDR_REG, dst_addr);
	REG_WRITE(bitblt_regs, SRC_ADDR_REG, src_addr);
	REG_WRITE(bitblt_regs, DST_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, X_WIDTH_REG, x_width);
	REG_WRITE(bitblt_regs, Y_WIDTH_REG, y_width);
	REG_WRITE(bitblt_regs, BG_COLOR_REG, bg_color);
	REG_WRITE(bitblt_regs, FG_COLOR_REG, fg_color);

	REG_WRITE(bitblt_regs, OP_REG, BITBLT_SOLID_FILL_CMD);

        REG_WRITE(bitblt_regs, CTRL0_REG, (BITBLT_CTRL0_START_MSK));

	wait_until(bitblt_regs);

	view_vram(0x30000000, 500 + 32, 50 + 32, 4, 4);
}

void
bitblt_fill_pattern(void *bitblt_regs)
{
	uint32_t src_addr, dst_addr;
	uint32_t dst_stride;
	uint32_t bg_color, fg_color;
	uint32_t x_width, y_width;

	src_addr = 0x31950000; //layer3 top address /dev/video0

	if ( 1 ) {
		void *src_vram;
		src_vram = map_regs(0x31950000, 2048 * 4 * 64);
		for( int i = 0 ; i < 2048 * 64; ++i ) {
			if (( i % 3 ) == 0 ) {
				REG_WRITE(src_vram, i * 4, 0xFF123456);
			} else {
				REG_WRITE(src_vram, i * 4, 0xFFFF0000 + i * 2);
			}
		}
		for( int i = 0 ; i < 9; ++i ) {
			uint32_t v;
			v = REG_READ(src_vram, i * 4);
			printf("%x ", v);
		}
		printf("\n");

		munmap(src_vram, 0);
	}

	dst_addr = 0x30000000 + 500 * 4 + 2048 * 4 * 50;

	dst_stride = 2048 * 4;
	x_width = 208 * 4 - 1;
	y_width = 208 - 1;
	fg_color = 0x00000000;
	bg_color = 0xFF123456;

	REG_WRITE(bitblt_regs, CTRL1_REG, 0x00000002);
	REG_WRITE(bitblt_regs, DST_ADDR_REG, dst_addr);
	REG_WRITE(bitblt_regs, SRC_ADDR_REG, src_addr);
	REG_WRITE(bitblt_regs, DST_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, SRC_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, X_WIDTH_REG, x_width);
	REG_WRITE(bitblt_regs, Y_WIDTH_REG, y_width);
	REG_WRITE(bitblt_regs, BG_COLOR_REG, bg_color);
	REG_WRITE(bitblt_regs, FG_COLOR_REG, fg_color);

        //REG_WRITE(bitblt_regs, ROP_REG, BITBLT_ROP_NOTSORNOTD_CMD);
        REG_WRITE(bitblt_regs, ROP_REG, BITBLT_ROP_S_CMD);
	REG_WRITE(bitblt_regs, OP_REG, BITBLT_PAT_FILL_TRANS_CMD);

        REG_WRITE(bitblt_regs, CTRL0_REG, (BITBLT_CTRL0_START_MSK | BITBLT_CTRL0_SRC_LIN_MSK));

	wait_until(bitblt_regs);

	view_vram(0x30000000, 500 + 32, 50, 8, 8);
}

void
bitblt_aa_font(void *bitblt_regs)
{
	uint32_t src_addr, dst_addr;
	uint32_t dst_stride;
	uint32_t bg_color, fg_color;
	uint32_t x_width, y_width;
	bool transparent;

	src_addr = 0x31950000; //layer3 top address /dev/video0
	dst_addr = 0x30000000 + 500 * 4 + 2048 * 50 * 4; // layer0 top address /dev/video3
	{
		void *vram;
		vram = map_regs(0x31950000, 2048 * 4 * 640);
		char *cp;
		cp = (char *)vram;
		for( int i = 0 ; i < 32 * 32; ++i ) {
			*cp = i % 2;
			++cp;
		}

		munmap(vram, 0);
	}

	REG_WRITE(bitblt_regs, 0x800 + 0, 0x11111111);
	REG_WRITE(bitblt_regs, 0x800 + 4, 0x77777777);

	dst_stride = 2048 * 4;
	x_width = 3 * 4 - 1;
	y_width = 3 - 1;
	fg_color = 0x00000000;
	bg_color = 0x00000000;
	transparent = false;

	REG_WRITE(bitblt_regs, CTRL1_REG, 0x00000005);
	REG_WRITE(bitblt_regs, DST_ADDR_REG, dst_addr);
	REG_WRITE(bitblt_regs, SRC_ADDR_REG, src_addr);
	REG_WRITE(bitblt_regs, DST_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, SRC_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, X_WIDTH_REG, x_width);
	REG_WRITE(bitblt_regs, Y_WIDTH_REG, y_width);
	REG_WRITE(bitblt_regs, BG_COLOR_REG, fg_color);
	REG_WRITE(bitblt_regs, FG_COLOR_REG, bg_color);

	REG_WRITE(bitblt_regs, OP_REG, BITBLT_AA_FONT_EXPAND_CMD);

        REG_WRITE(bitblt_regs, ROP_REG, BITBLT_ROP_WHITE_CMD);

        REG_WRITE(bitblt_regs, CTRL0_REG, (BITBLT_CTRL0_START_MSK | BITBLT_CTRL0_SRC_LIN_MSK));

	wait_until(bitblt_regs);

	view_vram(0x30000000, 500, 50, 4, 4);
}


void
bitblt_move(void *bitblt_regs)
{
	uint32_t src_addr, dst_addr;
	uint32_t dst_stride, src_stride;
	uint32_t bg_color, fg_color;
	uint32_t x_width, y_width;
	bool transparent;
	view_vram(0x30000000, 500 + 32, 50 + 32, 4, 4);

	src_addr = 0x30000000 + 500 * 4 + 2048 * 50 * 4; // layer0 top address /dev/video3
	dst_addr = 0x30000000 + (500 + 32) * 4 + 2048 * (50 + 32) * 4; // layer0 top address /dev/video3

	dst_stride = 2048 * 4;
	src_stride = 2048 * 4;
	x_width = 32 * 4 - 1;
	y_width = 32 - 1;
	fg_color = 0xFF0000FF;
	bg_color = 0xFF0000FF;
	transparent = false;

	REG_WRITE(bitblt_regs, CTRL1_REG, 0x00000000);
	REG_WRITE(bitblt_regs, DST_ADDR_REG, dst_addr);
	REG_WRITE(bitblt_regs, SRC_ADDR_REG, src_addr);
	REG_WRITE(bitblt_regs, DST_STRD_REG, dst_stride);
	REG_WRITE(bitblt_regs, SRC_STRD_REG, src_stride);
	REG_WRITE(bitblt_regs, X_WIDTH_REG, x_width);
	REG_WRITE(bitblt_regs, Y_WIDTH_REG, y_width);
	REG_WRITE(bitblt_regs, BG_COLOR_REG, bg_color);
	REG_WRITE(bitblt_regs, FG_COLOR_REG, fg_color);

	REG_WRITE(bitblt_regs, ROP_REG, PLUS);
	REG_WRITE(bitblt_regs, OP_REG, BITBLT_PORTER_DUFF_CMD);

        REG_WRITE(bitblt_regs, CTRL0_REG, (BITBLT_CTRL0_START_MSK));

	wait_until(bitblt_regs);

	view_vram(0x30000000, 500 + 32, 50 + 32, 4, 4);
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

	void *bitblt_regs;
	bitblt_regs = map_regs(LOGIBITBLT_REGS_BASE_ADDR, REGS_SIZE);
	if ( bitblt_regs == 0 ) {
		fprintf(stderr, "map_regs error %x\n", LOGIBITBLT_REGS_BASE_ADDR);
		goto err;
	}
	fprintf(stderr, "map_addr:0x%x\n", (uint32_t)bitblt_regs);

	bitblt_ip_version(bitblt_regs);

	bitblt_solid(bitblt_regs);
	bitblt_fill_pattern(bitblt_regs);
	/*
	bitblt_solid(bitblt_regs);
	bitblt_aa_font(bitblt_regs);
	bitblt_move(bitblt_regs);
	*/

	alarm(10);
	getchar();
	munmap(bitblt_regs, 0);
err:
	munmap(fbptr, screensize);

	return 0;
}
