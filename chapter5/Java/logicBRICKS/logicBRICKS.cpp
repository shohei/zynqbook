#include <cmath>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

#include <jni.h>

#include "logicBRICKS.h"
#include "logiBMP.h"

namespace {

void
throw_JavaException(JNIEnv *env, const char *class_name, const char *msg)
{
	jclass a_jclass;
	a_jclass = env->FindClass(class_name);
	env->ThrowNew(a_jclass, msg);
	env->DeleteLocalRef(a_jclass);
}

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

void *logiCVC_ML_regs;
void *logiBITBLT_regs;
void *logiBMP_regs;

//----------------------------------------------------------------
uint32_t read_reg32(void *reg_addr, uint32_t offset)
{
	off_t reg_addr_off_t(reinterpret_cast<off_t>(reg_addr));
	reg_addr_off_t += offset;
	volatile uint32_t *reg32(reinterpret_cast<volatile uint32_t *>(reg_addr_off_t));
	return *reg32;
}

//----------------------------------------------------------------
void write_reg32(void *reg_addr, uint32_t offset, uint32_t value)
{
	off_t reg_addr_off_t(reinterpret_cast<off_t>(reg_addr));
	reg_addr_off_t += offset;
	volatile uint32_t *reg32(reinterpret_cast<volatile uint32_t *>(reg_addr_off_t));
	*reg32 = value;
}

//----------------------------------------------------------------
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

//----------------------------------------------------------------

class Layer {
public:
	Layer(int a_layer_no, void *a_logicvc_regs_base, void *a_phy_addr, uint32_t a_size, uint32_t w, uint32_t h, uint32_t a_stride_bytes, uint32_t a_buffer_offset_bytes, uint32_t a_bytes_per_pixel) :
		layer_no(a_layer_no), phy_addr(a_phy_addr), size(a_size), mem_addr(0), width(w), height(h), stride_bytes(a_stride_bytes), buffer_offset_bytes(a_buffer_offset_bytes), bytes_per_pixel(a_bytes_per_pixel), read_buffer_no(0), write_buffer_no(0)
	{
		off_t layer_regs_offset_off_t;

		layer_regs_offset_off_t = (off_t) a_logicvc_regs_base;
		layer_regs_offset_off_t += ( layer_no ) * 0x80 +  0x100;
		layer_regs_base = (void *)layer_regs_offset_off_t;
	}

	void init() {
		mem_addr =  map_dev_mem((off_t)phy_addr, size);
	}

	int layer_no;
	void *layer_regs_base;
	void *phy_addr;
	uint32_t size;
	void *mem_addr;
	uint32_t width;
	uint32_t height;
	uint32_t stride_bytes;
	uint32_t buffer_offset_bytes;
	uint32_t bytes_per_pixel;
	uint32_t read_buffer_no;
	uint32_t write_buffer_no;

	void enable_layer(bool enable_flag) {
		bit_write_reg32(layer_regs_base, 0x38, 1, enable_flag?1:0);
	}
	void select_buffer(uint32_t buffer_no) {
		uint32_t value;

	        value = buffer_no & 0x3;
		value <<= (layer_no * 2);
		value |= (0x400 << layer_no);

		write_reg32(logiCVC_ML_regs, 0x58, value);
	}
	uint32_t get_buffer_no() {
	        uint32_t value;
		value = read_reg32(logiCVC_ML_regs, 0x58);
		return (value >> (layer_no * 2)) & 0x3;
	}

	bool is_buffer_switched()
	{
	        return (read_reg32(logiCVC_ML_regs, 0x68) & (1 << layer_no));
	}

	void clear_buffer_switched() {
	        write_reg32(logiCVC_ML_regs, 0x68, 1 << layer_no);
	}

	void *calc_addr(int x, int y) {
		off_t addr_off_t;
		addr_off_t = (off_t)phy_addr;
		addr_off_t += y * stride_bytes;
		addr_off_t += x * bytes_per_pixel;
		if ( write_buffer_no != 0 ) {
			addr_off_t += 1080 * stride_bytes;
		}
		return (void *)addr_off_t;
	}

	static Layer *layer[5];

	void change_buffer(uint32_t layer_no, uint32_t buffer_no) {
		Layer *l;
		l = layer[layer_no];
		uint32_t current_buffer_no;
		current_buffer_no = l->get_buffer_no();
		if ( buffer_no == current_buffer_no ) {
			return;
		}
		l->select_buffer(buffer_no);
		while(! l->is_buffer_switched()) {
		}
		l->clear_buffer_switched();
	}
}; // Layer

Layer *layer[5];
//----------------------------------------------------------------


} // namespace

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_init(JNIEnv *env, jclass obj)
{
	logiCVC_ML_regs = map_dev_mem(0x40030000, 0x6000);
	logiBITBLT_regs = map_dev_mem(0x40080000, 0x0800);
	logiBMP_regs = map_dev_mem(0x40090000, 0x0400);

	layer[0] = new Layer(0, logiCVC_ML_regs, (void *)0x338f4000, 8192 * 1080 * 3, 1024, 768, 8192, 8192 * 1080, 4);
	layer[0]->init();
	layer[1] = new Layer(1, logiCVC_ML_regs, (void *)0x31fa4000, 8192 * 1080 * 3, 1024, 768, 8192, 8192 * 1080, 4);
	layer[1]->init();
	layer[2] = new Layer(2, logiCVC_ML_regs, (void *)0x30000000, 8192 * 1080 * 3, 1024, 768, 8192, 8192 * 1080, 4);
	layer[2]->init();
	layer[3] = new Layer(3, logiCVC_ML_regs, (void *)0x31950000, 2048 * 1080 * 3, 1024, 768, 2048, 2048 * 1080, 1);
	layer[3]->init();
	layer[2]->enable_layer(true);
}

//----------------------------------------------------------------

JNIEXPORT jint JNICALL Java_logicBRICKS_map(JNIEnv *env, jclass obj, jint phy_addr, jint size)
{
	void *addr;

	addr = map_dev_mem(phy_addr, size);

	if ( addr == MAP_FAILED ) {
		throw_JavaException(env, "LJava/lang/RuntimeException", "cannot map /dev/mem");
		return -1;
	}
	return (jint)addr;
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_load_1image (JNIEnv *env, jclass obj, jint addr, jint w, jint h, jint stride_bytes, jint bytes_per_pixel, jbyteArray image)
{
	unsigned int y;
	off_t addr_off_t;
	jbyte *image_p, *bp;
	unsigned int w_bytes = w * bytes_per_pixel;

	addr_off_t = addr;

	image_p = env->GetByteArrayElements(image, 0);
	if (image_p == 0) {
		throw_JavaException(env, "LJava/lang/NullException", "cannot get array elements");
		return;
	}

	bp = image_p;
	for ( y = 0 ; y < h; ++y ) {
		memcpy((void *)addr_off_t, &bp[0], w_bytes);
		addr_off_t += stride_bytes;
		bp += w_bytes;
	}

	env->ReleaseByteArrayElements(image, image_p, JNI_ABORT);
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_box_1fill(JNIEnv *env, jclass obj, jint layer_no, jint x, jint y, jint w, jint h, jint color)
{
	while ((read_reg32(logiBITBLT_regs, 0) & 0x80) == 0x80);

	write_reg32(logiBITBLT_regs, 0x0004, 0x0006); // ARGB8888
	write_reg32(logiBITBLT_regs, 0x000C, 0x000C); // fill

	void *addr;
	Layer *dst_layer;
	dst_layer = layer[layer_no];
	addr = dst_layer->calc_addr(x, y);
	write_reg32(logiBITBLT_regs, 0x0014, (uint32_t)addr);
	write_reg32(logiBITBLT_regs, 0x001C, dst_layer->stride_bytes);
	write_reg32(logiBITBLT_regs, 0x002C, (uint32_t)color);

	write_reg32(logiBITBLT_regs, 0x0020, w * dst_layer->bytes_per_pixel-1);
	write_reg32(logiBITBLT_regs, 0x0024, h - 1);
	write_reg32(logiBITBLT_regs, 0x0000, 0x080);
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_copy_1image(JNIEnv *env, jclass obj, jint src_layer_no, jint src_x, jint src_y, jint w, jint h, jint dst_layer_no, jint dst_x, jint dst_y, jint mode)
{
	while ((read_reg32(logiBITBLT_regs, 0) & 0x80) == 0x80);

	write_reg32(logiBITBLT_regs, 0x0004, 0x0006); // ARGB8888
	write_reg32(logiBITBLT_regs, 0x0008, mode);
	write_reg32(logiBITBLT_regs, 0x000C, 0x0002); // move with rop
	//write_reg32(logiBITBLT_regs, 0x000C, 0x000C); // fill

	void *src_addr;
	Layer *src_layer;
	src_layer = layer[src_layer_no];
	src_addr = src_layer->calc_addr(src_x, src_y);
	write_reg32(logiBITBLT_regs, 0x0010, (uint32_t)src_addr);

	void *dst_addr;
	Layer *dst_layer;
	dst_layer = layer[dst_layer_no];
	dst_addr = dst_layer->calc_addr(dst_x, dst_y);
	write_reg32(logiBITBLT_regs, 0x0014, (uint32_t)dst_addr);

	write_reg32(logiBITBLT_regs, 0x0018, src_layer->stride_bytes);
	write_reg32(logiBITBLT_regs, 0x001C, dst_layer->stride_bytes);

	write_reg32(logiBITBLT_regs, 0x0020, w * dst_layer->bytes_per_pixel - 1);
	write_reg32(logiBITBLT_regs, 0x0024, h - 1);
	write_reg32(logiBITBLT_regs, 0x0000, 0x080);
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_rotate_1copy_1image(JNIEnv *env, jclass obj, jint src_layer_no, jint src_x, jint src_y, jint w, jint h, jint dst_layer_no, jint dst_x, jint dst_y, jint angle)
{
	float r_matrix_row0[2];
	float r_matrix_row1[2];
	float src_xy0[2], src_xy1[2], src_xy2[2], src_xy3[2];
	float uv0[2], uv1[2], uv2[2], uv3[2];
	float dst_xy0[2], dst_xy1[2], dst_xy2[2], dst_xy3[2];
	float center[2];
	float reduce = 0.75;

	r_matrix_row0[0] = cosf(angle * M_PI / 180.0) * reduce;
	r_matrix_row1[1] = r_matrix_row0[0];

	r_matrix_row0[1] = sinf(angle * M_PI / 180.0) * reduce;
	r_matrix_row1[0] = -r_matrix_row0[1];

	center[0] = dst_x + w / 2.0;
	center[1] = dst_y + h / 2.0;

	uv0[0] = dst_x - center[0];
	uv0[1] = -(dst_y - center[1]);

	dst_xy0[0] = uv0[0] * r_matrix_row0[0] + uv0[1] * r_matrix_row0[1];
	dst_xy0[0] += center[0];
	dst_xy0[1] = uv0[0] * r_matrix_row1[0] + uv0[1] * r_matrix_row1[1];
	dst_xy0[1] = -dst_xy0[1];
	dst_xy0[1] += center[1];

	uv1[0] = uv0[0] + w - 1.0;
	uv1[1] = uv0[1];
	
	dst_xy1[0] = uv1[0] * r_matrix_row0[0] + uv1[1] * r_matrix_row0[1];
	dst_xy1[0] += center[0];
	dst_xy1[1] = uv1[0] * r_matrix_row1[0] + uv1[1] * r_matrix_row1[1];
	dst_xy1[1] = -dst_xy1[1];
	dst_xy1[1] += center[1];

	uv2[0] = uv1[0];
	uv2[1] = uv1[1] - h + 1.0;
	
	dst_xy2[0] = uv2[0] * r_matrix_row0[0] + uv2[1] * r_matrix_row0[1];
	dst_xy2[0] += center[0];
	dst_xy2[1] = uv2[0] * r_matrix_row1[0] + uv2[1] * r_matrix_row1[1];
	dst_xy2[1] = -dst_xy2[1];
	dst_xy2[1] += center[1];

	uv3[0] = uv0[0];
	uv3[1] = uv2[1];

	dst_xy3[0] = uv3[0] * r_matrix_row0[0] + uv3[1] * r_matrix_row0[1];
	dst_xy3[0] += center[0];
	dst_xy3[1] = uv3[0] * r_matrix_row1[0] + uv3[1] * r_matrix_row1[1];
	dst_xy3[1] = -dst_xy3[1];
	dst_xy3[1] += center[1];

	/*
	dst_xy0[0] = (dst_xy0[0] / 3.0);
	dst_xy0[1] = (dst_xy0[1] / 3.0);
	dst_xy1[0] = (dst_xy1[0] / 3.0);
	dst_xy1[1] = (dst_xy1[1] / 3.0);
	dst_xy2[0] = (dst_xy2[0] / 3.0);
	dst_xy2[1] = (dst_xy2[1] / 3.0);
	dst_xy3[0] = (dst_xy3[0] / 3.0);
	dst_xy3[1] = (dst_xy3[1] / 3.0);
	*/
#if 0
	//----------------
	dst_xy0[0] = roundf(dst_xy0[0] * 8) / 8;
	dst_xy0[1] = roundf(dst_xy0[1] * 8) / 8;
	dst_xy1[0] = roundf(dst_xy1[0] * 8) / 8;
	dst_xy1[1] = roundf(dst_xy1[1] * 8) / 8;
	dst_xy2[0] = roundf(dst_xy2[0] * 8) / 8;
	dst_xy2[1] = roundf(dst_xy2[1] * 8) / 8;
	dst_xy3[0] = roundf(dst_xy3[0] * 8) / 8;
	dst_xy3[1] = roundf(dst_xy3[1] * 8) / 8;
#endif

	//----------------
	src_xy0[0] = src_x;
	src_xy0[1] = src_y;

	src_xy1[0] = src_x + w - 1.0;
	src_xy1[1] = src_y;

	src_xy2[0] = src_x + w - 1.0;
	src_xy2[1] = src_y + h - 1.0;

	src_xy3[0] = src_x;
	src_xy3[1] = src_y + h - 1.0;

	logiBMP::parameter param;

	void *src_addr;
	Layer *src_layer;
	src_layer = layer[src_layer_no];
	src_addr = src_layer->calc_addr(0, 0);

	param.texsize = 0;
	param.twidth = src_layer->width;
	param.theight = src_layer->height;
	param.tstride = src_layer->stride_bytes / 4;
	param.texptr = (uint32_t)((off_t)src_addr - (off_t)0x30000000);

	void *dst_addr;
	Layer *dst_layer;
	dst_layer = layer[dst_layer_no];
	dst_addr = dst_layer->calc_addr(0, 0);

	param.output_ptr = (uint32_t)((off_t)dst_addr - (off_t)0x30000000);
	param.outstride = dst_layer->stride_bytes / 4;

#if 0
	src_xy0[0] = 0.000;
	src_xy0[1] = 0.000;
	dst_xy0[0] = 779.589386;
	dst_xy0[1] = 213.904724;

	src_xy1[0] = 959.000;
	src_xy1[1] = 0.000;
	dst_xy1[0] = 1024.460999;
	dst_xy1[1] = 8.425629;

	src_xy2[0] = 959.000;
	src_xy2[1] = 539.000;
	dst_xy2[0] = 1139.941025;
	dst_xy2[1] = 146.054199;

	src_xy0[0] = 0.000;
	src_xy0[1] = 0.000;
	dst_xy0[0] = 357.436523;
	dst_xy0[1] = 483.904724;

	src_xy1[0] = 959.000;
	src_xy1[1] = 0.000;
	dst_xy1[0] = 602.308136;
	dst_xy1[1] = 278.425629;

	src_xy2[0] = 959.000;
	src_xy2[1] = 539.000;
	dst_xy2[0] = 602.308136;
	dst_xy2[1] = 416.054199;
#endif

	printf("-x---------\n");
	printf("(%f %f)-(%f %f)\n", dst_xy0[0], dst_xy0[1], dst_xy1[0], dst_xy1[1]);
	printf("(%f %f)-(%f %f)\n", dst_xy2[0], dst_xy2[1], dst_xy3[0], dst_xy3[1]);

	logiBMP::rotate_triangle(logiBMP_regs, src_xy0[0], src_xy0[1], src_xy1[0], src_xy1[1], src_xy2[0], src_xy2[1], dst_xy0[0], dst_xy0[1], dst_xy1[0], dst_xy1[1], dst_xy2[0], dst_xy2[1], &param);

	logiBMP::rotate_triangle(logiBMP_regs, src_xy0[0], src_xy0[1], src_xy2[0], src_xy2[1], src_xy3[0], src_xy3[1], dst_xy0[0], dst_xy0[1], dst_xy2[0], dst_xy2[1], dst_xy3[0], dst_xy3[1], &param);
	/*
	funny
	logiBMP::rotate_triangle(logiBMP_regs, src_xy0[0], src_xy0[1], src_xy1[0], src_xy1[1], src_xy3[0], src_xy3[1], dst_xy0[0], dst_xy0[1], dst_xy1[0], dst_xy1[1], dst_xy2[0], dst_xy2[1], &param);

	logiBMP::rotate_triangle(logiBMP_regs, src_xy1[0], src_xy1[1], src_xy2[0], src_xy2[1], src_xy3[0], src_xy3[1], dst_xy0[0], dst_xy0[1], dst_xy2[0], dst_xy2[1], dst_xy3[0], dst_xy3[1], &param);
	*/

	/*
	mirror ... no!!
	logiBMP::rotate_triangle(logiBMP_regs, src_xy0[0], src_xy0[1], src_xy1[0], src_xy1[1], src_xy3[0], src_xy3[1], dst_xy1[0], dst_xy1[1], dst_xy0[0], dst_xy0[1], dst_xy2[0], dst_xy2[1], &param);

	logiBMP::rotate_triangle(logiBMP_regs, src_xy1[0], src_xy1[1], src_xy2[0], src_xy2[1], src_xy3[0], src_xy3[1], dst_xy2[0], dst_xy2[1], dst_xy0[0], dst_xy0[1], dst_xy3[0], dst_xy3[1], &param);
	*/
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_enable_1layer (JNIEnv *env, jclass obj, jint layer_no, jboolean enable_flag)
{
	Layer *a_layer;
	a_layer = layer[layer_no];
	a_layer->enable_layer(enable_flag?true:false);
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_set_1buffer(JNIEnv *env, jclass obj, jint layer_no, jint buffer_kind, jint buffer_no)
{
	Layer *a_layer;
	a_layer = layer[layer_no];
	if ( buffer_kind == 0 ) {
		a_layer->read_buffer_no = 3 & buffer_no;
	} else {
		a_layer->write_buffer_no = 3 & buffer_no;
	}
}

//----------------------------------------------------------------
JNIEXPORT void JNICALL Java_logicBRICKS_switch_1buffer(JNIEnv *env, jclass obj, jint layer_no)
{
	Layer *a_layer;
	a_layer = layer[layer_no];
	uint32_t current_buffer_no;

	uint32_t tmp;
	tmp = a_layer->read_buffer_no;
	a_layer->read_buffer_no = a_layer->write_buffer_no;
	a_layer->write_buffer_no = tmp;

	current_buffer_no = a_layer->get_buffer_no();
	if ( current_buffer_no == a_layer->read_buffer_no ) {
		return;
	}

	a_layer->select_buffer(a_layer->read_buffer_no);
	while(! a_layer->is_buffer_switched()) {
	}
	a_layer->clear_buffer_switched();
}

//----------------------------------------------------------------
JNIEXPORT jboolean JNICALL Java_logicBRICKS_is_1busy_1bmp(JNIEnv *env, jclass obj)
{
	return (jboolean)((read_reg32(logiBMP_regs, 0) & 0x80) == 0);
}
//----------------------------------------------------------------
JNIEXPORT jboolean JNICALL Java_logicBRICKS_is_1busy_1bitblt (JNIEnv *env, jclass obj)
{
	return (jboolean)((read_reg32(logiBITBLT_regs, 0) & 0x80) == 0x80);
}
