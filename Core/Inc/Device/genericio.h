/**
 * @file genericio.h
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DEVICE_GENERIC_IO_H
#define DEVICE_GENERIC_IO_H

#include <stdbool.h>

typedef enum {
  GENERICIO_ID_1 = 0,
  GENERICIO_ID_2,
  GENERICIO_ID_3,
  GENERICIO_ID_4,
  GENERICIO_ID_5,
  GENERICIO_ID_6,
  GENERICIO_ID_MAX
} GENERICIO_Id;

void GENERICIO_init(void);
void GENERICIO_set(GENERICIO_Id id, bool enable);

#endif // DEVICE_GENERIC_IO_H