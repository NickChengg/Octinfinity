#include "math.h"

//Basic Trigonometry
static inline int16_t __sin(int32_t angle) {
	return arm_sin_q15((q15_t)(angle*TRIG_SCALE/SCALED_2_PI));
}
static inline int16_t __cos(int32_t angle) {
	return arm_cos_q15((q15_t)(angle*TRIG_SCALE/SCALED_2_PI));
}
static inline int16_t __tan(int32_t angle) {
	q15_t qngle = (q15_t)(angle*TRIG_SCALE/SCALED_2_PI);
	return (arm_sin_q15(qngle)<<15)/(arm_cos_q15(qngle));
}

int16_t sin(int32_t angle){
	angle %= SCALED_2_PI;
	while( angle<0){angle += SCALED_2_PI;}
	return __sin(angle);
}	
	
int16_t cos(int32_t angle){
	angle %= SCALED_2_PI;
	while(angle<0){angle += SCALED_2_PI;}
	return __cos(angle);
}

int32_t tan(int32_t in){
	while(in > SCALED_PI){in -= SCALED_PI;}
	while(in < 0){in += SCALED_PI;}
	return __tan(in);
}

//Inverse Trigonometry
int32_t asin(int16_t sin_val) {
	int32_t search_sin = 0;

	int32_t lower_bound = 0, upper_bound = FAST_MATH_TABLE_SIZE/4;
	int32_t interval = FAST_MATH_TABLE_SIZE/8, index = 0;
	
	uint8_t sign = (sin_val<0);
	if (sign) sin_val = -sin_val;
	sign = (sin_val>0) - sign;

	// uint8_t sign = SIGN(sin_val);
	// sin_val = ABS(sin_val);

	while (interval>1)
	{
		index = lower_bound+interval;
		search_sin = sinTable_q15[index];

		if (sin_val == search_sin) break;
		else   if (sin_val < search_sin)   upper_bound = index;
		else /*if (sin_val > search_sin)*/ lower_bound = index;
		interval = (upper_bound - lower_bound) / 2;
	}

	int32_t lower_sin_val = sinTable_q15[lower_bound],
	        upper_sin_val = sinTable_q15[upper_bound];

	// int32_t real_angle = (int32_t) index*ANGLE_STEP;
	int32_t real_angle = lower_bound*ANGLE_STEP + ((int32_t)sin_val-lower_sin_val)*ANGLE_STEP/(upper_sin_val-lower_sin_val);

	return sign*real_angle;
}

int32_t acos(int16_t cos_val) {
	return SCALED_PI_2-asin(cos_val);
}

// Get the octant a coordinate pair is in.
#define OCTANTIFY(_x, _y, _o)   do {                            \
    int _t; _o= 0;                                              \
    if(_y<  0)  {            _x= -_x;   _y= -_y; _o += 4; }     \
    if(_x<= 0)  { _t= _x;    _x=  _y;   _y= -_t; _o += 2; }     \
    if(_x<=_y)  { _t= _y-_x; _x= _x+_y; _y=  _t; _o += 1; }     \
} while(0);

int32_t atan2(int32_t x, int32_t y) {
    if (x == 0) {
		if (y < 0)        return -SCALED_PI_2;
		else if (y == 0)  return 0;
		else              return SCALED_PI_2;
	} else if (y == 0) {
		if (x < 0)        return SCALED_PI;
		else              return  0;
	}

    int32_t  phi, t, t2, dphi;

    OCTANTIFY(x, y, phi);
    phi *= SCALED_PI_4;

    t= (y<<15)/x;
    t2= -t*t>>15;

    dphi= 0x0470; //Tweaked Taylor Expansion Series
    dphi= 0x1029 + (t2*dphi>>15);
    dphi= 0x1F0B + (t2*dphi>>15);
    dphi= 0x364C + (t2*dphi>>15);
    dphi= 0xA2FC + (t2*dphi>>15);
    dphi= dphi*t>>15;
    return phi + ((dphi+4)>>3);
}

int32_t atan(int32_t t) {
    int32_t  t2, dphi;

    t2= -t*t>>15;

    dphi= 0x0470; //Tweaked Taylor Expansion Series
    dphi= 0x1029 + (t2*dphi>>15);
    dphi= 0x1F0B + (t2*dphi>>15);
    dphi= 0x364C + (t2*dphi>>15);
    dphi= 0xA2FC + (t2*dphi>>15);
    dphi= dphi*t>>15;
    return SIGN(t)*((dphi+4)>>3);
}

/**
  * @brief  Enhanced rapid sqrt approximation tested in 2^31 range (cmath sqrt function bugs on > 2^31)
  *					with maximum 0.101663% deviation at sqrt(11) minimum -0.000000000045339% at sqrt(24813001) and 
  *					average 0.000000559339% deviation
  *					Larger input generally results in smaller output error
  * @param  v:	Input limited to 2^31 by variable type
  * @retval Scaled value of 1024*sqrt(v)
	*	@attention		To CS members: 		Don't bother changing things not understandable unless O(f{n}) < O(1)
	*								To CPEG members:	Float is used here only because it dramatically shortens sqrt 
	*																	time compared to other implementations in Int, try proving the
	*																	magic number 0x233b4000 by relating to bits meaning of Float. 
	*																	Final result is accurate due to refinment by Newton's Iteration.
	*								To EE members:		This is much faster
  */
uint32_t sqrt(int32_t v) {
	// q31_t sqrt_val;
	// arm_sqrt_q31(v, &sqrt_val);
	// return sqrt_val;

	union
	{
		uint32_t tmp;
		float f;
	} u;

	v = ABS(v);
	//Represent number as a float
	u.f = v;
	//Magic floating pt bithacks gives an estimate of the sqrt
	u.tmp = (uint32_t)u.f;
	//Represent the number as an u32
	u.tmp = (uint32_t)(0x233b4000 + (u.tmp >> 1));
	//Newton method - 2 iterations
	u.tmp = (u.tmp + (uint64_t)v*16384/u.tmp + 1)/2;
	u.tmp = (u.tmp + (uint64_t)v*16384/u.tmp + 1)/2;

	return u.tmp * 8;
}
