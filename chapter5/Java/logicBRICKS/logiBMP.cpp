#include <stdio.h>
#include <cmath>
#include "logiBMP.h"
#include "utils.h"

namespace logiBMP {

void
set_param(float u0, float v0, float u1, float v1, float u2, float v2, float x0, float y0, float x1, float y1, float x2, float y2, logiBMP::parameter *param) 
{
	float tmp_u, tmp_v;
	float tmp_x, tmp_y, tmp_w;

	unsigned int k, kk;
	unsigned int rk, rrk;

	if ( y1 < y0 ) {
		if ( y2 < y1 ) {
			rk = 2;
		} else {
			rk = 1;
		}
	} else {
		if ( y2 < y0 ) {
			if ( y0 == y1 ) {
				if ( x0 < x1 ) {
					// mirror? or error?
					rk = 1;
				} else {
					rk = 2;
				}
			} else {
				rk = 2;
			}
		} else {
			if ( y2 == y0 ) {
				if ( x2 < x0 ) {
					rk = 2;
				} else {
					// mirror? or error?
					rk = 0;
				}
			} else {
				rk = 0;
			}
		}
	}

	if ( roundf(y1) < roundf(y0) ) {
		if ( roundf(y2) < roundf(y1) ) {
			rrk = 2;
		} else {
			rrk = 1;
		}
	} else {
		if ( roundf(y2) < roundf(y0) ) {
			if ( roundf(y0) == roundf(y1) ) {
				if ( roundf(x0) < roundf(x1) ) {
					// mirror? or error?
					rrk = 1;
				} else {
					rrk = 2;
				}
			} else {
				rrk = 2;
			}
		} else {
			if ( roundf(y2) == roundf(y0) ) {
				if ( roundf(x2) < roundf(x0) ) {
					rrk = 2;
				} else {
					// mirror? or error?
					rrk = 0;
				}
			} else {
				rrk = 0;
			}
		}
	}
	printf("rk %d =? rrk %d\n", rk, rrk);

	switch ( rk ) {
	case 2:
		tmp_u = u0;
		tmp_v = v0;
		tmp_x = x0;
		tmp_y = y0;

		u0 = u2;
		v0 = v2;
		x0 = x2;
		y0 = y2;

		u2 = u1;
		v2 = v1;
		x2 = x1;
		y2 = y1;

		u1 = tmp_u;
		v1 = tmp_v;
		x1 = tmp_x;
		y1 = tmp_y;
		
		break;
	case 1:
		tmp_u = u0;
		tmp_v = v0;
		tmp_x = x0;
		tmp_y = y0;

		u0 = u1;
		v0 = v1;
		x0 = x1;
		y0 = y1;

		u1 = u2;
		v1 = v2;
		x1 = x2;
		y1 = y2;

		u2 = tmp_u;
		v2 = tmp_v;
		x2 = tmp_x;
		y2 = tmp_y;
		break;
	default:
		// assert( rk == 0);
		break;
	}

#if 0
	kk = 0;
	for (k = 0 ; k < 2; k++ ) {
		if ( y1 < y0 ) {
			kk ++;
			//printf("rotate 1 %f %f\n", y1, y0);

			tmp_u = u0;
			tmp_v = v0;
			tmp_x = x0;
			tmp_y = y0;

			u0 = u1;
			v0 = v1;
			x0 = x1;
			y0 = y1;

			u1 = u2;
			v1 = v2;
			x1 = x2;
			y1 = y2;

			u2 = tmp_u;
			v2 = tmp_v;
			x2 = tmp_x;
			y2 = tmp_y;
		} else {
			break;
		}
	}

	kk *= 10;
	if ( y2 < y0 ) {
		kk += 2;
		printf("rotate 2 %f %f\n", y2, y0);
		tmp_u = u0;
		tmp_v = v0;
		tmp_x = x0;
		tmp_y = y0;

		u0 = u2;
		v0 = v2;
		x0 = x2;
		y0 = y2;

		u2 = u1;
		v2 = v1;
		x2 = x1;
		y2 = y1;

		u1 = tmp_u;
		v1 = tmp_v;
		x1 = tmp_x;
		y1 = tmp_y;
	}

	kk *= 10;
	if ( y2 == y0 ) {
		//printf("rotate 3 pre %f %f\n", y2, y0);
		if ( x2 < x0 ) {
			kk += 2;
			printf("rotate 3 %f %f\n", x2, x0);
			tmp_u = u1;
			tmp_v = v1;
			tmp_x = x1;
			tmp_y = y1;

			u1 = u0;
			v1 = v0;
			x1 = x0;
			y1 = y0;

			u0 = u2;
			v0 = v2;
			x0 = x2;
			y0 = y2;

			u2 = tmp_u;
			v2 = tmp_v;
			x2 = tmp_x;
			y2 = tmp_y;
		}
	}

	printf("k:%d kk:%03d\n", k, kk);
#endif

	tmp_x =  (y1 - y2);
	tmp_y = -(y0 - y2);
	tmp_w =  (y0 - y1);

	float K, N, R;
	R = tmp_x      + tmp_y      + tmp_w     ;
	K = tmp_x * u0 + tmp_y * u1 + tmp_w * u2;
	N = tmp_x * v0 + tmp_y * v1 + tmp_w * v2;
	
	tmp_x = -(x1 - x2);
	tmp_y =  (x0 - x2);
	tmp_w = -(x0 - x1);

	float L, P, S;
	S = tmp_x      + tmp_y      + tmp_w     ;
	L = tmp_x * u0 + tmp_y * u1 + tmp_w * u2;
	P = tmp_x * v0 + tmp_y * v1 + tmp_w * v2;

	tmp_x =  ((x1 * y2) - (y1 * x2));
	tmp_y = -((x0 * y2) - (y0 * x2));
	tmp_w =  ((x0 * y1) - (y0 * x1));

	float M, Q, T;
	T = tmp_x      + tmp_y      + tmp_w     ;
	M = tmp_x * u0 + tmp_y * u1 + tmp_w * u2;
	Q = tmp_x * v0 + tmp_y * v1 + tmp_w * v2;

	param->x0 = float_to_uint32_t<N_FACTOR>(x0);
	param->y0 = float_to_uint32_t<N_FACTOR>(y0);
	param->u0 = float_to_uint32_t<N_FACTOR>(u0);
	param->v0 = float_to_uint32_t<N_FACTOR>(v0);
	param->x1 = float_to_uint32_t<N_FACTOR>(x1);
	param->y1 = float_to_uint32_t<N_FACTOR>(y1);
	param->x2 = float_to_uint32_t<N_FACTOR>(x2);
	param->y2 = float_to_uint32_t<N_FACTOR>(y2);

	float d_f, tmp_f;
	d_f = 2.0  * ( R * x0 + S * y0 + T );

	param->d_init = float_to_uint32_t<NN_FACTOR>(d_f);

	tmp_f = 2.0  * ( R * u0 - K );
	param->au_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	tmp_f = 2.0  * ( S * u0 - L );
	param->bu_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	tmp_f = d_f * u0 - 2.0  * ( K * x0 + L * y0 + M );
	param->eu_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	tmp_f = 2.0  * ( R * v0 - N );
	param->av_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	tmp_f = 2.0  * ( R * v0 - P );
	param->bv_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	tmp_f = d_f * v0 - 2.0  * ( N * x0 + P * y0 + Q );
	param->ev_init = float_to_uint32_t<NN_FACTOR>(tmp_f);

	param->tr_init = float_to_uint32_t<NN_FACTOR>(2.0 * R);
	param->ts_init = float_to_uint32_t<NN_FACTOR>(2.0 * S);

	{
		float xf[4];
		float yf[4];
		int32_t r_y1;
		r_y1 = roundf(y1);

		xf[0] = x0;
		yf[0] = y0;

		if (roundf(y0) == r_y1) {
			xf[1] = x1;
			yf[1] = y1;
			xf[2] = x2;
			yf[2] = y2;
			xf[3] = x2;
			yf[3] = y2;
		} else if (r_y1 == roundf(y2)) {
			xf[1] = x2;
			yf[1] = y2;
			xf[2] = x1;
			yf[2] = y1;
			xf[3] = x1;
			yf[3] = y1;
		} else {
			xf[1] = x0;
			yf[1] = y0;
			xf[2] = x2;
			yf[2] = y2;
			xf[3] = x1;
			yf[3] = y1;
		}

		float err_x[4];
		float err_y[4];
		float tmp_p;

		err_x[0] = modff(1.5 - modff(xf[0], &tmp_p), &tmp_p);
		err_x[1] = modff(1.5 - modff(xf[1], &tmp_p), &tmp_p);
		err_x[2] = modff(1.5 - modff(xf[2], &tmp_p), &tmp_p);
		err_x[3] = modff(1.5 - modff(xf[3], &tmp_p), &tmp_p);

		err_y[0] = modff(1.5 - modff(yf[0], &tmp_p), &tmp_p);
		err_y[1] = modff(1.5 - modff(yf[1], &tmp_p), &tmp_p);
		err_y[2] = modff(1.5 - modff(yf[2], &tmp_p), &tmp_p);
		err_y[3] = modff(1.5 - modff(yf[3], &tmp_p), &tmp_p);

		param->err0 = float_to_uint32_t<N_FACTOR>(((xf[2] - xf[0]) * err_y[0]) - ((yf[2] - yf[0]) * err_x[0]));
		param->err1 = float_to_uint32_t<N_FACTOR>(((xf[3] - xf[1]) * err_y[1]) - ((yf[3] - yf[1]) * err_x[1]));

		if (roundf(yf[2]) < roundf(yf[3])) {
			param->err2 = float_to_uint32_t<N_FACTOR>(((xf[3] - xf[2]) * err_y[2]) - ((yf[3] - yf[2]) * err_x[2]));
		} else {
			param->err2 = float_to_uint32_t<N_FACTOR>(((xf[2] - xf[3]) * err_y[3]) - ((yf[2] - yf[3]) * err_x[3]));
		}
	}

	param->err0 = 0;
	param->err1 = 0;
	param->err2 = 0;
}

void 
rotate_triangle(void *logiBMP_regs, float u0, float v0, float u1, float v1, float u2, float v2, float x0, float y0, float x1, float y1, float x2, float y2, logiBMP::parameter *param) 
{
	set_param(u0, v0, u1, v1, u2, v2, x0, y0, x1, y1, x2, y2, param) ;

	uint32_t offset;
	offset = 4;
	uint32_t *uip;
	uip = reinterpret_cast<uint32_t *>(param);

	/* 0x0000 */
	//nop
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0010 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0020 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0030 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0040 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0050 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	/* skip */
	uip++;
	offset += 4;

	/* 0x0060 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* 0x0070 */
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;
	write_reg32(logiBMP_regs, offset, *uip++);
	offset += 4;

	/* doit */
	while ((read_reg32(logiBMP_regs, 0) & 0x80) == 0);

	write_reg32(logiBMP_regs, 0, 0x80 | 0x10 | 0x04 | 0x40000000);
}

} // namespace logiBMP
