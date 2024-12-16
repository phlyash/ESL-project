#ifndef FLASH_MODULE_H 
#define FLASH_MODULE_H
#include "led_module.h"

#define READ_START_ADDRESS 0x79000
#define READ_END_ADDRESS READ_START_ADDRESS + NRF_DFU_APP_DATA_AREA_SIZE - 0x4
#define CONFIG_WORDS_USED 1

#define HUE_MASK 0x1FF
#define HUE_POS 0x0

#define SATURATION_MASK 0x1FE00
#define SATURATION_POS HUE_POS + 0x9

#define VALUE_MASK 0x1FE0000
#define VALUE_POS SATURATION_POS + 0x8

void save_color(hsv_t* hsv);
void load_color(hsv_t* hsv);
void init_flash(void);

#endif