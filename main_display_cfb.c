/*
 * Copyright (c) 2018 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <display/cfb.h>
#include <stdio.h>

#ifndef DISPLAY_DRIVER
#define DISPLAY_DRIVER		DT_LABEL(DT_INST(0, ilitek_ili9340))
#endif

void main(void)
{
	const struct device *dev;
	uint16_t rows;
	uint8_t ppt;
	uint8_t font_width;
	uint8_t font_height;

	dev = device_get_binding(DISPLAY_DRIVER);

	if (dev == NULL) {
		printf("Device not found\n");
		return;
	}

	if (display_set_pixel_format(dev, PIXEL_FORMAT_RGB_565) != 0) {
		printf("Failed to set required pixel format\n");
		return;
	}

	printf("initialized %s\n", DISPLAY_DRIVER);

	if (cfb_framebuffer_init(dev)) {
		printf("Framebuffer initialization failed!\n");
		return;
	}

	cfb_framebuffer_clear(dev, true);

	display_blanking_off(dev);

	rows = cfb_get_display_parameter(dev, CFB_DISPLAY_ROWS);
	ppt = cfb_get_display_parameter(dev, CFB_DISPLAY_PPT);

	for (int idx = 0; idx < 42; idx++) {
		if (cfb_get_font_size(dev, idx, &font_width, &font_height)) {
			break;
		}
		cfb_framebuffer_set_font(dev, idx);
		printf("font width %d, font height %d\n",
		       font_width, font_height);
	}

	printf("x_res %d, y_res %d, ppt %d, rows %d, cols %d\n",
	       cfb_get_display_parameter(dev, CFB_DISPLAY_WIDTH),
	       cfb_get_display_parameter(dev, CFB_DISPLAY_HEIGH),
	       ppt,
	       rows,
	       cfb_get_display_parameter(dev, CFB_DISPLAY_COLS));

	while (1) {
		for (int i = 0; i < rows; i++) {
			cfb_framebuffer_clear(dev, false);
			if (cfb_print(dev,
				      "0123456789mMgj!\"§$%&/()=",
				      0, i * ppt)) {
				printf("Failed to print a string\n");
				continue;
			}

			cfb_framebuffer_finalize(dev);
			k_msleep(1000);
		}
	}
}