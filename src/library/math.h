#ifndef __MATH_H
#define __MATH_H

#include "stm32f10x.h"

#ifndef ARM_MATH_CM3
	#define ARM_MATH_CM3
#endif
#include "arm_math.h"
#include "arm_common_tables.h"

#define ABS(a) ((a)<0?-(a):(a))
#define SIGN(a) (((a) > 0) - ((a) < 0))

#define SCALED_2_PI 36000
#define SCALED_PI   18000
#define SCALED_PI_2 9000
#define SCALED_PI_4 9000
#define ANGLE_STEP  70 //SCALED_2_PI / FAST_MATH_TABLE_SIZE
#define TRIG_SCALE  32768

/**
  * @brief  Enhanced rapid sqrt approximation tested in 2^31 range (cmath sqrt function bugs on > 2^31)
  *					with maximum 0.101663% deviation at sqrt(11) minimum -0.000000000045339% at sqrt(24813001) and 
  *					average 0.000000559339% deviation
  *					Larger input generally results in smaller output error
  * @param  v:	Input limited to 2^31 by variable type
  * @retval Scaled value of 1024*sqrt(v)
  */
uint32_t sqrt(int32_t v);

/** Approximate a sine value.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return sin(angle) scaled by 32768 (-32768 ~ 32767)
*/
int16_t sin(int32_t angle);

/** Approximate a cosine value.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return cos(angle) scaled by 32768 (-32768 ~ 32767)
*/
int16_t cos(int32_t angle);

/** Approximate a tan value.
	Be careful value around multiple of 90 deg will be very inaccurate.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return tan(angle) scaled by 32768
*/
int32_t tan(int32_t angle);

/**
 * @brief      Approximate a inverse sin value
 * @param[in]  sin_val  The sin value, scaled by 32768
 * @return     asin(s) scaled by 100 (0 ~ 9000)
 */
int32_t asin(int16_t sin_val);

/**
 * @brief      Approximate a inverse cos value
 * @param[in]  sin_val  The cos value, scaled by 32768
 * @return     acos(s) scaled by 100 (0 ~ 9000)
 */
int32_t acos(int16_t cos_val);

/**
 * @brief      Approximate the atan2 function (atan(y/x) keeping track of quadrant)
 * @param[in]  x     x coordinate
 * @param[in]  y     y coordinate
 * @return     atan2(x,y) aka angle component of polar form of (x,y) 
 * 				scaled by 100 (0 ~ 36000)
 */
int32_t atan2(int32_t x, int32_t y);

/**
 * @brief      Approximate a inverse tan value
 *
 * @param[in]  tan_val  The tangent value, scaled by 32768
 * @return     atan(t) scaled by 100 (-9000 ~ 9000)
 */
int32_t atan(int32_t tan_val);

#endif
