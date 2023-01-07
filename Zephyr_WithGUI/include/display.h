#ifndef DISPLAY_PROJECT_H
#define DISPLAY_PROJECT_H

#include "main.h"

#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))

uint8_t init_display();
uint8_t init_gui(void (*btn_event_func_ptr)(lv_obj_t*,lv_event_t));

#endif