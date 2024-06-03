/*
 * sound.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_SOUND_H_
#define INC_DEVICE_SOUND_H_

#include <stdbool.h>
#include <stdint.h>

bool SOUND_init(void);
bool SOUND_run(void);
bool SOUND_play(uint8_t file);
bool SOUND_isError();
void SOUND_test(void);

#endif /* INC_DEVICE_SOUND_H_ */
