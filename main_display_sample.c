/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * Based on ST7789V sample:
 * Copyright (c) 2019 Marc Reilly
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(sample, LOG_LEVEL_INF);

#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>

#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))

enum corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT
};

typedef void (*fill_buffer)(enum corner corner, uint8_t grey, uint8_t *buf,
			    size_t buf_size);

static uint16_t get_rgb565_color(enum corner corner, uint8_t grey)
{
	uint16_t color = 0;
	uint16_t grey_5bit;

	switch (corner) {
	case TOP_LEFT:
		color = 0xF800u;
		break;
	case TOP_RIGHT:
		color = 0x07E0u;
		break;
	case BOTTOM_RIGHT:
		color = 0x001Fu;
		break;
	case BOTTOM_LEFT:
		grey_5bit = grey & 0x1Fu;
		/* shift the green an extra bit, it has 6 bits */
		color = grey_5bit << 11 | grey_5bit << (5 + 1) | grey_5bit;
		break;
	}
	return color;
}

static void fill_buffer_rgb565(enum corner corner, uint8_t grey, uint8_t *buf,
			       size_t buf_size)
{
	uint16_t color = get_rgb565_color(corner, grey);

	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(buf + idx + 0) = (color >> 8) & 0xFFu;
		*(buf + idx + 1) = (color >> 0) & 0xFFu;
	}
}

void main(void)
{
	size_t x;
	size_t y;
	size_t rect_w;
	size_t rect_h;
	size_t h_step;
	size_t scale;
	size_t grey_count;
	uint8_t *buf;
	int32_t grey_scale_sleep;
	const struct device *display_dev;
	struct display_capabilities capabilities;
	struct display_buffer_descriptor buf_desc;
	size_t buf_size = 0;
	fill_buffer fill_buffer_fnc = NULL;

	LOG_INF("Display sample for %s", DISPLAY_DEV_NAME);

	display_dev = device_get_binding(DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("Device %s not found. Aborting sample.",
			DISPLAY_DEV_NAME);
		return;
	}

	display_get_capabilities(display_dev, &capabilities);

	LOG_ERR("Screen info :\n[X,Y]=[%d,%d]\nPixel Format: %d",capabilities.x_resolution, capabilities.y_resolution, capabilities.current_pixel_format);
	if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED) {
		rect_w = 16;
		rect_h = 8;
	} else {
		rect_w = 2;
		rect_h = 1;
	}

	h_step = rect_h;
	scale = (capabilities.x_resolution / 8) / rect_h;

	rect_w *= scale;
	rect_h *= scale;

	if (capabilities.screen_info & SCREEN_INFO_EPD) {
		grey_scale_sleep = 10000;
	} else {
		grey_scale_sleep = 100;
	}

	buf_size = rect_w * rect_h;

	if (buf_size < (capabilities.x_resolution * h_step)) {
		buf_size = capabilities.x_resolution * h_step;
	}

	if (capabilities.current_pixel_format == PIXEL_FORMAT_RGB_565){
		fill_buffer_fnc = fill_buffer_rgb565;
		buf_size *= 2;
	} else  {
		LOG_ERR("Device does not allow 'PIXEL_FORMAT_RGB_565' pixel format. Aborting.");
		return;
	}

	buf = k_malloc(buf_size);

	if (buf == NULL) {
		LOG_ERR("Could not allocate memory. Aborting sample.");
		return;
	}

	(void)memset(buf, 0xFFu, buf_size);

	buf_desc.buf_size = buf_size;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = h_step;

	for (int idx = 0; idx < capabilities.y_resolution; idx += h_step) {
		display_write(display_dev, 0, idx, &buf_desc, buf);
	}

	buf_desc.pitch = rect_w;
	buf_desc.width = rect_w;
	buf_desc.height = rect_h;

	fill_buffer_fnc(TOP_LEFT, 0, buf, buf_size);
	x = 0;
	y = 0;
	display_write(display_dev, x, y, &buf_desc, buf);

	fill_buffer_fnc(TOP_RIGHT, 0, buf, buf_size);
	x = capabilities.x_resolution - rect_w;
	y = 0;
	display_write(display_dev, x, y, &buf_desc, buf);

	fill_buffer_fnc(BOTTOM_RIGHT, 0, buf, buf_size);
	x = capabilities.x_resolution - rect_w;
	y = capabilities.y_resolution - rect_h;
	display_write(display_dev, x, y, &buf_desc, buf);

	display_blanking_off(display_dev);

	grey_count = 0;
	x = 0;
	y = capabilities.y_resolution - rect_h;

	while (1) {
		fill_buffer_fnc(BOTTOM_LEFT, grey_count, buf, buf_size);
		display_write(display_dev, x, y, &buf_desc, buf);
		++grey_count;
		k_msleep(grey_scale_sleep);
	}

	return;
}