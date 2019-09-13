/*
 * utils.h
 *
 *  Created on: 04.08.2019
 *      Author: kowal
 */

#ifndef USER_UTILS_H_
#define USER_UTILS_H_

#define MIN(A,B)		(((A) < (B)) ? (A) : (B))
#define MAX(A,B)		(((A) > (B)) ? (A) : (B))
#define ABS(A)			((A < 0) ? (-A) : A)
#define ABS_DIFF(A, B)	(MAX(A, B) - MIN(A, B))

#endif /* USER_UTILS_H_ */
