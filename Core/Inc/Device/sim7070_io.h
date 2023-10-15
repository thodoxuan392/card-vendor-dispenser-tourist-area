/*
 * sim7070_io.h
 *
 *  Created on: Oct 15, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_SIM7070_IO_H_
#define INC_DEVICE_SIM7070_IO_H_


void SIM7070_IO_init();
bool SIM7070_power(bool enable);
bool SIM7070_reset(bool enable);
bool SIM7070_is_net();

#endif /* INC_DEVICE_SIM7070_IO_H_ */
