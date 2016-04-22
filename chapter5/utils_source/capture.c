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

//----------------------------------------------------------------
void
capture(uint32_t *addr, uint32_t screensize, char *file_name)
{
	int fds;
	int rv;
	fds = open(file_name, O_RDWR | O_TRUNC | O_CREAT, 0666);

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
	fprintf(stderr, "Usage: capture /dev/fbN file\n");
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
	if ( argc != 3 ) {
		Usage();
		exit(1);
	}

	{
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

	screensize = line_len * yres;

	//----------------------------------------------------------------
	fbptr = (char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE,MAP_SHARED,fds,0);
	if ( (int)fbptr == -1 ) {
		fprintf(stderr, "Don't get framebuffer device to memory !\n");
		exit(4);
	}
	close(fds);

	//----------------------------------------------------------------
	{
		char *file_name;
		file_name = argv[2];
		capture((uint32_t *)fbptr, screensize, file_name);
	}

	munmap(fbptr, screensize);

	return 0;
}

