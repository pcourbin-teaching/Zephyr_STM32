#include "display.hpp"
#include "ESIEALogo.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_display);

void myDisplay::task_handler()
{
    lv_task_handler();
}

void myDisplay::lv_create_logo()
{
    /*Image From variable*/
    logo = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(ESIEALogo);
    lv_img_set_src(logo, &ESIEALogo);
    lv_obj_align(logo, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_update_layout(logo);
}

void myDisplay::lv_create_btn_clear()
{
    /* https://docs.lvgl.io/8.3/widgets/core/btn.html */
    /*Init the style for the default state*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_GREEN, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, (lv_style_prop_t)0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);

    btn_clear = lv_btn_create(lv_scr_act()); /*Add a button the current screen*/
    lv_obj_remove_style_all(btn_clear);      /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_clear, &style, 0);
    lv_obj_add_style(btn_clear, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_pos(btn_clear, 5, 5);                                         /*Set its position*/
    lv_obj_set_size(btn_clear, 50, 50);                                      /*Set its size*/
    lv_obj_add_event_cb(btn_clear, btn_clear_event, LV_EVENT_CLICKED, this); /*Assign a callback to the button*/

    lv_obj_t *label = lv_label_create(btn_clear); /*Add a label to the button*/
    lv_label_set_text(label, "Clear");            /*Set the labels text*/
    lv_obj_center(label);
}

static void btn_clear_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    myDisplay *display = (myDisplay *)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED)
    {
        /*Clear the text*/
        display->text_clear();
        display->chart_clear();
    }
}

void myDisplay::lv_create_text()
{
    text = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(text, &lv_font_montserrat_12, 0);
    int y_base = logo->coords.y2;
    if (chart != NULL)
    {
        y_base = chart->coords.y2;
    }
    lv_obj_set_pos(text, 10, y_base + 5);

    lv_obj_update_layout(text); // To get the height of the text, 0 otherwize.
    text_nb_max = (lv_obj_get_height(lv_scr_act()) - lv_obj_get_y(text)) / lv_obj_get_height(text) - 1;
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(text, text_value);
    lv_obj_update_layout(text);
}

void myDisplay::lv_create_chart()
{
    /*Create a chart*/
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, CHART_DEFAULT_WIDTH, CHART_DEFAULT_HEIGHT);

    lv_obj_set_pos(chart, (lv_obj_get_width(lv_scr_act()) - CHART_DEFAULT_WIDTH) / 2, logo->coords.y2 + 5);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(chart, CHART_DEFAULT_NUMBER_POINTS);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, CHART_DEFAULT_PRIMARY_Y_MIN, CHART_DEFAULT_PRIMARY_Y_MAX);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, CHART_DEFAULT_SECONDARY_Y_MIN, CHART_DEFAULT_SECONDARY_Y_MAX);

    /*Add two data series*/
    serie_temperature = lv_chart_add_series(chart, lv_palette_main(CHART_DEFAULT_TEMPERATURE_COLOR), LV_CHART_AXIS_PRIMARY_Y);
    serie_humidity = lv_chart_add_series(chart, lv_palette_main(CHART_DEFAULT_HUMIDITY_COLOR), LV_CHART_AXIS_SECONDARY_Y);

    lv_obj_update_layout(chart);
}

void myDisplay::init(const struct device *display_dev, bool with_chart)
{
    dev = display_dev;
    if (!device_is_ready(dev))
    {
        LOG_ERR("Could not get %s device.", dev->name);
        return;
    }
    display_set_orientation(dev, DISPLAY_ORIENTATION_ROTATED_180);

    lv_create_logo();
    lv_create_btn_clear();
    if (with_chart)
    {
        lv_create_chart();
    }
    lv_create_text();
    display_blanking_off(dev);
}

void myDisplay::chart_add_temperature(long value)
{
    if (chart != NULL)
        lv_chart_set_next_value(chart, serie_temperature, value);
}

void myDisplay::chart_add_humidity(long value)
{
    if (chart != NULL)
        lv_chart_set_next_value(chart, serie_humidity, value);
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

void myDisplay::chart_clear()
{
    if (chart != NULL)
    {
        /*Remove the two data series*/
        lv_chart_remove_series(chart, serie_temperature);
        lv_chart_remove_series(chart, serie_humidity);

        /*Add the two data series*/
        serie_temperature = lv_chart_add_series(chart, lv_palette_main(CHART_DEFAULT_TEMPERATURE_COLOR), LV_CHART_AXIS_PRIMARY_Y);
        serie_humidity = lv_chart_add_series(chart, lv_palette_main(CHART_DEFAULT_HUMIDITY_COLOR), LV_CHART_AXIS_SECONDARY_Y);

        lv_obj_update_layout(chart);
    }
}