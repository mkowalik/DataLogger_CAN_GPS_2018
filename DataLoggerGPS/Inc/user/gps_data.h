/*
 * gps_data.h
 *
 *  Created on: 10.07.2019
 *      Author: Michal Kowalik
 */

#ifndef USER_GPS_DATA_H_
#define USER_GPS_DATA_H_

#include "stdint.h"
#include "date_time_data.h"
#include "user/fixed_point.h"

typedef enum {
	GPSFixType_NoFix = 1,
	GPSFixType_2DFix = 2,
	GPSFixType_3DFix = 3
} GPSFixType;

typedef struct {
	DateTime_TypeDef	dateTime;
	uint32_t			msTime;
	FixedPoint			longitude;
	FixedPoint			latitude;
	uint8_t				nSatellites;
	FixedPoint			altitude; //< in meters
	FixedPoint			speed; //< in km/h
	GPSFixType			fixType;//< GPGSA
	FixedPoint			horizontalPrecision;//< GPGSA
	FixedPoint			verticalPrecision; //< GPGSA
} GPSData_TypeDef;



#endif /* USER_GPS_DATA_H_ */
