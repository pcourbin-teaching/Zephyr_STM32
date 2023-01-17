#include "display.hpp"
#include "ESIEALogo.hpp"

#include <logging/log.h>
LOG_MODULE_DECLARE(app);

void myDisplay::task_handler()
{
    lv_task_handler();
}

void myDisplay::lv_create_logo()
{
    /*Image From variable*/
    logo = lv_img_create(lv_scr_act(), NULL);
    LV_IMG_DECLARE(ESIEALogo);
    lv_img_set_src(logo, &ESIEALogo);
    lv_obj_align(logo, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
}

void myDisplay::lv_create_text()
{
    text = lv_label_create(lv_scr_act(), NULL);
    lv_area_t coords;
    if (chart == NULL)
    {
        lv_obj_get_coords(logo, &coords);
        lv_obj_set_pos(text, 10, coords.y2 + 5);
    }
    else
    {
        lv_obj_get_coords(chart, &coords);
        lv_obj_set_pos(text, 10, coords.y2 + 5);
    }
    text_nb_max = (lv_obj_get_height(lv_scr_act()) - lv_obj_get_y(text)) / lv_obj_get_height(text) - 1;
    lv_label_set_align(text, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(text, text_value);
}

void myDisplay::lv_create_chart()
{
    /*Create a chart*/
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, CHART_DEFAULT_WIDTH, CHART_DEFAULT_HEIGHT);

    lv_area_t coords;
    lv_obj_get_coords(logo, &coords);
    lv_obj_set_pos(chart, (lv_obj_get_width(lv_scr_act()) - lv_obj_get_width(chart)) / 2, coords.y2 + 5);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(chart, CHART_DEFAULT_NUMBER_POINTS);

    lv_chart_set_y_range(chart, LV_CHART_AXIS_PRIMARY_Y, CHART_DEFAULT_PRIMARY_Y_MIN, CHART_DEFAULT_PRIMARY_Y_MAX);
    lv_chart_set_y_range(chart, LV_CHART_AXIS_SECONDARY_Y, CHART_DEFAULT_SECONDARY_Y_MIN, CHART_DEFAULT_SECONDARY_Y_MAX);

    /*Add two data series*/
    serie_temperature = lv_chart_add_series(chart, CHART_DEFAULT_TEMPERATURE_COLOR);
    serie_humidity = lv_chart_add_series(chart, CHART_DEFAULT_HUMIDITY_COLOR);
    lv_chart_set_series_axis(chart, serie_humidity, LV_CHART_AXIS_SECONDARY_Y);
}

void myDisplay::init(bool with_chart)
{
    dev = device_get_binding(DISPLAY_DEV_NAME);
    if (dev == NULL)
    {
        LOG_ERR("Could not get %s device.", DISPLAY_DEV_NAME);
        return;
    }
    display_set_orientation(dev, DISPLAY_ORIENTATION_ROTATED_180);

    lv_create_logo();
    if (with_chart)
    {
        lv_create_chart();
    }
    lv_create_text();
    display_blanking_off(dev);
}

void myDisplay::chart_add_temperature(long value)
{
    lv_chart_set_next(chart, serie_temperature, value);
}

void myDisplay::chart_add_humidity(long value)
{
    lv_chart_set_next(chart, serie_humidity, value);
}

void myDisplay::text_add(char *text_to_add)
{
    if (strlen(text_value) > TEXT_MAX_SIZE - strlen(text_to_add) - 2 || text_nb > text_nb_max)
    {
        text_clear();
    }
    text_nb++;
    strcat(text_value, text_to_add);
    strcat(text_value, "\n");
    lv_label_set_text(text, text_value);
}

void myDisplay::text_clear()
{
    text_nb = 0;
    strcpy(text_value, "");
    lv_label_set_text(text, text_value);
}