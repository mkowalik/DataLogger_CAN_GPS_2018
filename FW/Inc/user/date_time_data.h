/*
 * date_time_data.h
 *
 *  Created on: 10.07.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_DATE_TIME_DATA_H_
#define USER_DATE_TIME_DATA_H_

typedef struct {
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
	uint16_t	miliseconds;
} DateTime_TypeDef;

#endif /* USER_DATE_TIME_DATA_H_ */
