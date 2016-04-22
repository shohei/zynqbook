#ifndef __LOGI_BMP_H__
#define __LOGI_BMP_H__

#include <stdint.h>

#define FRACTION 3
#define N_FACTOR ( 1 << FRACTION )
#define NN_FACTOR ( 1 << (FRACTION * 2) )
#define N_MASK (N_FACTOR - 1)

namespace logiBMP {

template <uint32_t n_factor>
uint32_t float_to_uint32_t(float __f__)
{
	float f0;
	f0 = __f__ * n_factor;
	int32_t i32;
	i32 = static_cast<int32_t>(f0);
	return static_cast<uint32_t>(i32);
}

struct parameter {
	uint32_t x0;
	uint32_t y0;
	uint32_t u0;
	uint32_t v0;
	uint32_t x1;
	uint32_t y1;
	uint32_t x2;
	uint32_t y2;
	uint32_t texptr;
	uint32_t texsize;
	uint32_t output_ptr;
	uint32_t d_init;
	uint32_t au_init;
	uint32_t bu_init;
	uint32_t eu_init;
	uint32_t av_init;
	uint32_t bv_init;
	uint32_t ev_init;
	uint32_t fg_col;
	uint32_t tr_init;
	uint32_t ts_init;
	uint32_t outstride;
	uint32_t __you_can_not_use_ip_version__;
	uint32_t err0;
	uint32_t err1;
	uint32_t err2;
	uint32_t twidth;
	uint32_t theight;
	uint32_t tstride;
};

void 
set_param(float u0, float v0, float u1, float v1, float u2, float v2, float x0, float y0, float x1, float y1, float x2, float y2, parameter *param);

void 
rotate_triangle(void *logiBMP_regs, float u0, float v0, float u1, float v1, float u2, float v2, float x0, float y0, float x1, float y1, float x2, float y2, logiBMP::parameter *param);

} // namespace logiBMP

#endif /* __LOGI_BMP_H__ */
