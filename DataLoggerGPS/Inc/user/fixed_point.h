/*
 * fixed_point.h
 *
 *  Created on: 05.05.2017
 *      Author: kowalik
 */

#ifndef FIXED_POINT_H_
#define FIXED_POINT_H_

#include "stdint.h"

//TODO for construction FixedPoint objects

typedef struct {
	int32_t	integer;
	uint8_t	fractionalBits;
} FixedPoint;

FixedPoint FixedPoint_constr(int32_t valueConverted, int32_t divider, int32_t multiplier, int32_t offset, uint8_t fractionalBits);
uint8_t FixedPoint_a_equal_b(FixedPoint a, FixedPoint b);
uint8_t FixedPoint_a_lessorequal_b(FixedPoint a, FixedPoint b);
uint8_t FixedPoint_a_greaterorequal_b(FixedPoint a, FixedPoint b);
uint8_t FixedPoint_a_greater_b(FixedPoint a, FixedPoint b);
uint8_t FixedPoint_a_less_b(FixedPoint a, FixedPoint b);

#endif /* FIXED_POINT_H_ */

