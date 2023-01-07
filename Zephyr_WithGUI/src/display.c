
#include "display.h"

LOG_MODULE_REGISTER(display);

extern uint32_t count;

lv_obj_t* btn = NULL;
lv_obj_t* hello_world_label = NULL;
lv_obj_t* count_label = NULL;
const struct device *display_dev = NULL;

uint8_t init_display(){
	uint8_t returned = 0;
    display_dev = device_get_binding(DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device not found. Aborting test.");
        returned = 1;
	}
	return returned;
}

uint8_t init_gui(void (*btn_event_func_ptr)(lv_obj_t*,lv_event_t)){
    uint8_t returned = 0;

    returned = init_display();
    if (!returned){
        return returned;
    }

	hello_world_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

	btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_set_event_cb(btn, (*btn_event_func_ptr));                 /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/

	display_blanking_off(display_dev);
    return returned;
}

