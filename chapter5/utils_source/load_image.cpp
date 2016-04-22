#include <iostream>

#include <cstring>

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
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

//----------------------------------------------------------------
void
Usage()
{
	fprintf(stderr, "Usage: load_image /dev/fbN rgb_raw_file width height\n");
}

//----------------------------------------------------------------

int
main(int argc, char **argv)
{
	int i;
	char *rgb_raw_file_name;
	int width, height;

	int fds ;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	if ( argc != 5 ) {
		Usage();
		exit(1);
	}
	//----------------------------------------------------------------
	if ( argv[1][0] == '/' ) {
		int no;

		// 01234567
		// /dev/fbN
		device_name[7] = argv[1][7];
		no = argv[1][7] - 0x30;
		if ((no < 0 ) || ( 4 < no )) {
			Usage();
			exit(2);
		}
	} else {
		Usage();
		exit(3);
	}
	i = 2;
	rgb_raw_file_name = argv[i++];
	width = atoi(argv[i++]);
	height = atoi(argv[i++]);

	fds = open(device_name, O_RDWR);

	if ( fds < 0 ) {
		perror("frame buffer:");
		goto err_exit;
	}

	//----------------------------------------------------------------
	if ( ioctl( fds , FBIOGET_FSCREENINFO , &finfo ) ) {
		perror("FBIOGET_FSCREENINFO");
		goto err_ioctl;
	}

	if ( ioctl( fds , FBIOGET_VSCREENINFO , &vinfo ) ) {
		perror("FBIOGET_VSCREENINFO");
		goto err_ioctl;
	}

	//----------------------------------------------------------------
	int src_fds;
	src_fds = open(rgb_raw_file_name, O_RDWR);
	if ( src_fds < 0 ) {
		perror(rgb_raw_file_name);
		goto err_open_rgb_raw_file_name;
	}
	void *src_image;
	unsigned int width_bytes;
	width_bytes = width * vinfo.bits_per_pixel / 8;
	//printf("width %d\n", width_bytes);

	src_image = mmap(0, width_bytes * height, PROT_READ | PROT_WRITE, MAP_PRIVATE, src_fds, 0);
	close(src_fds);
	if ( src_image == MAP_FAILED ) {
		perror(rgb_raw_file_name);
		goto err_mem_map_image;
	}

	//----------------------------------------------------------------
	void *vram;
	vram = mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
	close(fds);
	fds = -1;

	if ( vram == MAP_FAILED ) {
		perror("mmap");
		goto err_mem_map;
	}

	off_t vram_offset, src_offset;
	vram_offset = reinterpret_cast<off_t>(vram);
	src_offset = reinterpret_cast<off_t>(src_image);

	for( int y = 0 ; y < height ; ++y ) {
		void *srcp, *dstp;
		srcp = reinterpret_cast<void *>(src_offset);
		dstp = reinterpret_cast<void *>(vram_offset);
		memcpy(dstp, srcp, width_bytes);
		vram_offset += finfo.line_length;
		src_offset += width_bytes;
	}

	munmap(vram, 0);
	munmap(src_image, 0);

	return 0;

err_mem_map:
err_mem_map_image:
err_open_rgb_raw_file_name:
err_ioctl:
	if ( fds >= 0 ) {
		close(fds);
	}
err_exit:
	return 255;
}

