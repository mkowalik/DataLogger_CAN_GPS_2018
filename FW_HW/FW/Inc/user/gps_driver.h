/*
 * gps_driver.h
 *
 *  Created on: 12.09.2019
 *      Author: kowal
 */

#ifndef USER_GPS_DRIVER_H_
#define USER_GPS_DRIVER_H_

#include "main.h"

#if (defined(USED_GPS) && (USED_GPS == UBLOX_8M_GPS))
#include "ublox_m8_gps_driver.h"
typedef Ublox8MGPSDriver_TypeDef GPSDriver_TypeDef;
#elif (defined(USED_GPS) && (USED_GPS == SIM28_GPS))
#include "sim28_gps_driver.h"
typedef SIM28GPSDriver_TypeDef GPSDriver_TypeDef;
#endif

#endif /* USER_GPS_DRIVER_H_ */
