/*
 * Copyright (C) 2011-2012 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/fsl_devices.h>
#include <mach/mipi_csi2.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-int-device.h>
#include "mxc_v4l2_capture.h"

#define OV5640_VOLTAGE_ANALOG               2800000
#define OV5640_VOLTAGE_DIGITAL_CORE         1500000
#define OV5640_VOLTAGE_DIGITAL_IO           1800000

#define MIN_FPS 15
#define MAX_FPS 30
#define DEFAULT_FPS 30

#define OV5640_XCLK_MIN 6000000
#define OV5640_XCLK_MAX 24000000

enum ov5640_mode {
	ov5640_mode_MIN = 0,
	ov5640_mode_VGA_640_480 = 0,
	ov5640_mode_QVGA_320_240 = 1,
	ov5640_mode_NTSC_720_480 = 2,
	ov5640_mode_PAL_720_576 = 3,
	ov5640_mode_720P_1280_720 = 4,
	ov5640_mode_1080P_1920_1080 = 5,
	ov5640_mode_QSXGA_2592_1944 = 6,
	ov5640_mode_QCIF_176_144 = 7,
	ov5640_mode_MAX = 7,
	ov5640_mode_INIT = 0xff, /*only for sensor init*/
};

enum ov5640_frame_rate {
	ov5640_15_fps,
	ov5640_30_fps
};

struct reg_value {
	u16 u16RegAddr;
	u8 u8Val;
	u8 u8Mask;
	u32 u32Delay_ms;
};

struct ov5640_mode_info {
	enum ov5640_mode mode;
	u32 width;
	u32 height;
	struct reg_value *init_data_ptr;
	u32 init_data_size;
};

/*!
 * Maintains the information on the current state of the sesor.
 */
struct sensor_data ov5640_data;

static struct reg_value ov5640_init_setting_30fps_VGA[] = {

	{0x3103, 0x11, 0, 0}, {0x3008, 0x82, 0, 5}, {0x3008, 0x42, 0, 0},
	{0x3103, 0x03, 0, 0}, {0x3017, 0x00, 0, 0}, {0x3018, 0x00, 0, 0},
	{0x3034, 0x18, 0, 0}, {0x3035, 0x14, 0, 0}, {0x3036, 0x38, 0, 0},
	{0x3037, 0x13, 0, 0}, {0x3108, 0x01, 0, 0}, {0x3630, 0x36, 0, 0},
	{0x3631, 0x0e, 0, 0}, {0x3632, 0xe2, 0, 0}, {0x3633, 0x12, 0, 0},
	{0x3621, 0xe0, 0, 0}, {0x3704, 0xa0, 0, 0}, {0x3703, 0x5a, 0, 0},
	{0x3715, 0x78, 0, 0}, {0x3717, 0x01, 0, 0}, {0x370b, 0x60, 0, 0},
	{0x3705, 0x1a, 0, 0}, {0x3905, 0x02, 0, 0}, {0x3906, 0x10, 0, 0},
	{0x3901, 0x0a, 0, 0}, {0x3731, 0x12, 0, 0}, {0x3600, 0x08, 0, 0},
	{0x3601, 0x33, 0, 0}, {0x302d, 0x60, 0, 0}, {0x3620, 0x52, 0, 0},
	{0x371b, 0x20, 0, 0}, {0x471c, 0x50, 0, 0}, {0x3a13, 0x43, 0, 0},
	{0x3a18, 0x00, 0, 0}, {0x3a19, 0xf8, 0, 0}, {0x3635, 0x13, 0, 0},
	{0x3636, 0x03, 0, 0}, {0x3634, 0x40, 0, 0}, {0x3622, 0x01, 0, 0},
	{0x3c01, 0xa4, 0, 0}, {0x3c04, 0x28, 0, 0}, {0x3c05, 0x98, 0, 0},
	{0x3c06, 0x00, 0, 0}, {0x3c07, 0x08, 0, 0}, {0x3c08, 0x00, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x3000, 0x00, 0, 0},
	{0x3002, 0x1c, 0, 0}, {0x3004, 0xff, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x300e, 0x45, 0, 0}, {0x302e, 0x08, 0, 0}, {0x4300, 0x3f, 0, 0},
	{0x501f, 0x00, 0, 0}, {0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0},
	{0x440e, 0x00, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x4837, 0x0a, 0, 0}, {0x4800, 0x04, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5000, 0xa7, 0, 0}, {0x5001, 0xa3, 0, 0}, {0x5180, 0xff, 0, 0},
	{0x5181, 0xf2, 0, 0}, {0x5182, 0x00, 0, 0}, {0x5183, 0x14, 0, 0},
	{0x5184, 0x25, 0, 0}, {0x5185, 0x24, 0, 0}, {0x5186, 0x09, 0, 0},
	{0x5187, 0x09, 0, 0}, {0x5188, 0x09, 0, 0}, {0x5189, 0x88, 0, 0},
	{0x518a, 0x54, 0, 0}, {0x518b, 0xee, 0, 0}, {0x518c, 0xb2, 0, 0},
	{0x518d, 0x50, 0, 0}, {0x518e, 0x34, 0, 0}, {0x518f, 0x6b, 0, 0},
	{0x5190, 0x46, 0, 0}, {0x5191, 0xf8, 0, 0}, {0x5192, 0x04, 0, 0},
	{0x5193, 0x70, 0, 0}, {0x5194, 0xf0, 0, 0}, {0x5195, 0xf0, 0, 0},
	{0x5196, 0x03, 0, 0}, {0x5197, 0x01, 0, 0}, {0x5198, 0x04, 0, 0},
	{0x5199, 0x6c, 0, 0}, {0x519a, 0x04, 0, 0}, {0x519b, 0x00, 0, 0},
	{0x519c, 0x09, 0, 0}, {0x519d, 0x2b, 0, 0}, {0x519e, 0x38, 0, 0},
	{0x5381, 0x1e, 0, 0}, {0x5382, 0x5b, 0, 0}, {0x5383, 0x08, 0, 0},
	{0x5384, 0x0a, 0, 0}, {0x5385, 0x7e, 0, 0}, {0x5386, 0x88, 0, 0},
	{0x5387, 0x7c, 0, 0}, {0x5388, 0x6c, 0, 0}, {0x5389, 0x10, 0, 0},
	{0x538a, 0x01, 0, 0}, {0x538b, 0x98, 0, 0}, {0x5300, 0x08, 0, 0},
	{0x5301, 0x30, 0, 0}, {0x5302, 0x10, 0, 0}, {0x5303, 0x00, 0, 0},
	{0x5304, 0x08, 0, 0}, {0x5305, 0x30, 0, 0}, {0x5306, 0x08, 0, 0},
	{0x5307, 0x16, 0, 0}, {0x5309, 0x08, 0, 0}, {0x530a, 0x30, 0, 0},
	{0x530b, 0x04, 0, 0}, {0x530c, 0x06, 0, 0}, {0x5480, 0x01, 0, 0},
	{0x5481, 0x08, 0, 0}, {0x5482, 0x14, 0, 0}, {0x5483, 0x28, 0, 0},
	{0x5484, 0x51, 0, 0}, {0x5485, 0x65, 0, 0}, {0x5486, 0x71, 0, 0},
	{0x5487, 0x7d, 0, 0}, {0x5488, 0x87, 0, 0}, {0x5489, 0x91, 0, 0},
	{0x548a, 0x9a, 0, 0}, {0x548b, 0xaa, 0, 0}, {0x548c, 0xb8, 0, 0},
	{0x548d, 0xcd, 0, 0}, {0x548e, 0xdd, 0, 0}, {0x548f, 0xea, 0, 0},
	{0x5490, 0x1d, 0, 0}, {0x5580, 0x02, 0, 0}, {0x5583, 0x40, 0, 0},
	{0x5584, 0x10, 0, 0}, {0x5589, 0x10, 0, 0}, {0x558a, 0x00, 0, 0},
	{0x558b, 0xf8, 0, 0}, {0x5800, 0x23, 0, 0}, {0x5801, 0x14, 0, 0},
	{0x5802, 0x0f, 0, 0}, {0x5803, 0x0f, 0, 0}, {0x5804, 0x12, 0, 0},
	{0x5805, 0x26, 0, 0}, {0x5806, 0x0c, 0, 0}, {0x5807, 0x08, 0, 0},
	{0x5808, 0x05, 0, 0}, {0x5809, 0x05, 0, 0}, {0x580a, 0x08, 0, 0},
	{0x580b, 0x0d, 0, 0}, {0x580c, 0x08, 0, 0}, {0x580d, 0x03, 0, 0},
	{0x580e, 0x00, 0, 0}, {0x580f, 0x00, 0, 0}, {0x5810, 0x03, 0, 0},
	{0x5811, 0x09, 0, 0}, {0x5812, 0x07, 0, 0}, {0x5813, 0x03, 0, 0},
	{0x5814, 0x00, 0, 0}, {0x5815, 0x01, 0, 0}, {0x5816, 0x03, 0, 0},
	{0x5817, 0x08, 0, 0}, {0x5818, 0x0d, 0, 0}, {0x5819, 0x08, 0, 0},
	{0x581a, 0x05, 0, 0}, {0x581b, 0x06, 0, 0}, {0x581c, 0x08, 0, 0},
	{0x581d, 0x0e, 0, 0}, {0x581e, 0x29, 0, 0}, {0x581f, 0x17, 0, 0},
	{0x5820, 0x11, 0, 0}, {0x5821, 0x11, 0, 0}, {0x5822, 0x15, 0, 0},
	{0x5823, 0x28, 0, 0}, {0x5824, 0x46, 0, 0}, {0x5825, 0x26, 0, 0},
	{0x5826, 0x08, 0, 0}, {0x5827, 0x26, 0, 0}, {0x5828, 0x64, 0, 0},
	{0x5829, 0x26, 0, 0}, {0x582a, 0x24, 0, 0}, {0x582b, 0x22, 0, 0},
	{0x582c, 0x24, 0, 0}, {0x582d, 0x24, 0, 0}, {0x582e, 0x06, 0, 0},
	{0x582f, 0x22, 0, 0}, {0x5830, 0x40, 0, 0}, {0x5831, 0x42, 0, 0},
	{0x5832, 0x24, 0, 0}, {0x5833, 0x26, 0, 0}, {0x5834, 0x24, 0, 0},
	{0x5835, 0x22, 0, 0}, {0x5836, 0x22, 0, 0}, {0x5837, 0x26, 0, 0},
	{0x5838, 0x44, 0, 0}, {0x5839, 0x24, 0, 0}, {0x583a, 0x26, 0, 0},
	{0x583b, 0x28, 0, 0}, {0x583c, 0x42, 0, 0}, {0x583d, 0xce, 0, 0},
	{0x5025, 0x00, 0, 0}, {0x3a0f, 0x30, 0, 0}, {0x3a10, 0x28, 0, 0},
	{0x3a1b, 0x30, 0, 0}, {0x3a1e, 0x26, 0, 0}, {0x3a11, 0x60, 0, 0},
	{0x3a1f, 0x14, 0, 0}, {0x3008, 0x02, 0, 0}, {0x3c00, 0x04, 0, 300},
};

static struct reg_value ov5640_setting_30fps_VGA_640_480[] = {

	{0x3035, 0x14, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x04, 0, 0}, {0x380f, 0x38, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x0e, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0}, {0x3503, 0x00, 0, 0},
};

static struct reg_value ov5640_setting_15fps_VGA_640_480[] = {
	{0x3035, 0x22, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_30fps_QVGA_320_240[] = {
	{0x3035, 0x14, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x01, 0, 0}, {0x3809, 0x40, 0, 0}, {0x380a, 0x00, 0, 0},
	{0x380b, 0xf0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_15fps_QVGA_320_240[] = {
	{0x3035, 0x22, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x01, 0, 0}, {0x3809, 0x40, 0, 0}, {0x380a, 0x00, 0, 0},
	{0x380b, 0xf0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_30fps_QCIF_176_144[] = {
	{0x3035, 0x14, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x00, 0, 0}, {0x3809, 0xb0, 0, 0}, {0x380a, 0x00, 0, 0},
	{0x380b, 0x90, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};
static struct reg_value ov5640_setting_15fps_QCIF_176_144[] = {
	{0x3035, 0x22, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x00, 0, 0}, {0x3809, 0xb0, 0, 0}, {0x380a, 0x00, 0, 0},
	{0x380b, 0x90, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_30fps_NTSC_720_480[] = {
	{0x3035, 0x12, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x3c, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_15fps_NTSC_720_480[] = {
	{0x3035, 0x22, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x3c, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_30fps_PAL_720_576[] = {
	{0x3035, 0x12, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0x40, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x38, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_15fps_PAL_720_576[] = {
	{0x3035, 0x22, 0, 0}, {0x3036, 0x38, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0x40, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x38, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0xa3, 0, 0},
};

static struct reg_value ov5640_setting_30fps_720P_1280_720[] = {
	{0x3008, 0x42, 0, 0},
	{0x3035, 0x21, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x07, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0xfa, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0}, {0x3807, 0xa9, 0, 0},
	{0x3808, 0x05, 0, 0}, {0x3809, 0x00, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0xd0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x64, 0, 0},
	{0x380e, 0x02, 0, 0}, {0x380f, 0xe4, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x02, 0, 0},
	{0x3a03, 0xe4, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0xbc, 0, 0},
	{0x3a0a, 0x01, 0, 0}, {0x3a0b, 0x72, 0, 0}, {0x3a0e, 0x01, 0, 0},
	{0x3a0d, 0x02, 0, 0}, {0x3a14, 0x02, 0, 0}, {0x3a15, 0xe4, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x02, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0},
	{0x3824, 0x04, 0, 0}, {0x5001, 0x83, 0, 0}, {0x4005, 0x1a, 0, 0},
	{0x3008, 0x02, 0, 0}, {0x3503, 0,    0, 0},
};

static struct reg_value ov5640_setting_15fps_720P_1280_720[] = {
	{0x3035, 0x41, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x07, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0xfa, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0}, {0x3807, 0xa9, 0, 0},
	{0x3808, 0x05, 0, 0}, {0x3809, 0x00, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0xd0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x64, 0, 0},
	{0x380e, 0x02, 0, 0}, {0x380f, 0xe4, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x02, 0, 0},
	{0x3a03, 0xe4, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0xbc, 0, 0},
	{0x3a0a, 0x01, 0, 0}, {0x3a0b, 0x72, 0, 0}, {0x3a0e, 0x01, 0, 0},
	{0x3a0d, 0x02, 0, 0}, {0x3a14, 0x02, 0, 0}, {0x3a15, 0xe4, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x4713, 0x02, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0},
	{0x3824, 0x04, 0, 0}, {0x5001, 0x83, 0, 0},
};

static struct reg_value ov5640_setting_30fps_1080P_1920_1080[] = {
	{0x3008, 0x42, 0, 0},
	{0x3035, 0x21, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0}, {0x3814, 0x11, 0, 0},
	{0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x00, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9f, 0, 0},
	{0x3808, 0x0a, 0, 0}, {0x3809, 0x20, 0, 0}, {0x380a, 0x07, 0, 0},
	{0x380b, 0x98, 0, 0}, {0x380c, 0x0b, 0, 0}, {0x380d, 0x1c, 0, 0},
	{0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x04, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x21, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x06, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0x83, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x54, 0, 0}, {0x3c07, 0x07, 0, 0}, {0x3c08, 0x00, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3800, 0x01, 0, 0}, {0x3801, 0x50, 0, 0}, {0x3802, 0x01, 0, 0},
	{0x3803, 0xb2, 0, 0}, {0x3804, 0x08, 0, 0}, {0x3805, 0xef, 0, 0},
	{0x3806, 0x05, 0, 0}, {0x3807, 0xf1, 0, 0}, {0x3808, 0x07, 0, 0},
	{0x3809, 0x80, 0, 0}, {0x380a, 0x04, 0, 0}, {0x380b, 0x38, 0, 0},
	{0x380c, 0x09, 0, 0}, {0x380d, 0xc4, 0, 0}, {0x380e, 0x04, 0, 0},
	{0x380f, 0x60, 0, 0}, {0x3612, 0x2b, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3a02, 0x04, 0, 0}, {0x3a03, 0x60, 0, 0}, {0x3a08, 0x01, 0, 0},
	{0x3a09, 0x50, 0, 0}, {0x3a0a, 0x01, 0, 0}, {0x3a0b, 0x18, 0, 0},
	{0x3a0e, 0x03, 0, 0}, {0x3a0d, 0x04, 0, 0}, {0x3a14, 0x04, 0, 0},
	{0x3a15, 0x60, 0, 0}, {0x4713, 0x02, 0, 0}, {0x4407, 0x04, 0, 0},
	{0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0}, {0x3824, 0x04, 0, 0},
	{0x4005, 0x1a, 0, 0}, {0x3008, 0x02, 0, 0},
	{0x3503, 0, 0, 0},
};

static struct reg_value ov5640_setting_15fps_1080P_1920_1080[] = {
	{0x3008, 0x42, 0, 0},
	{0x3035, 0x21, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0}, {0x3814, 0x11, 0, 0},
	{0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x00, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9f, 0, 0},
	{0x3808, 0x0a, 0, 0}, {0x3809, 0x20, 0, 0}, {0x380a, 0x07, 0, 0},
	{0x380b, 0x98, 0, 0}, {0x380c, 0x0b, 0, 0}, {0x380d, 0x1c, 0, 0},
	{0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x04, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x21, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x06, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0x83, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x54, 0, 1}, {0x3c07, 0x07, 0, 0}, {0x3c08, 0x00, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3800, 0x01, 0, 0}, {0x3801, 0x50, 0, 0}, {0x3802, 0x01, 0, 0},
	{0x3803, 0xb2, 0, 0}, {0x3804, 0x08, 0, 0}, {0x3805, 0xef, 0, 0},
	{0x3806, 0x05, 0, 0}, {0x3807, 0xf1, 0, 0}, {0x3808, 0x07, 0, 0},
	{0x3809, 0x80, 0, 0}, {0x380a, 0x04, 0, 0}, {0x380b, 0x38, 0, 0},
	{0x380c, 0x09, 0, 0}, {0x380d, 0xc4, 0, 0}, {0x380e, 0x04, 0, 0},
	{0x380f, 0x60, 0, 0}, {0x3612, 0x2b, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3a02, 0x04, 0, 0}, {0x3a03, 0x60, 0, 0}, {0x3a08, 0x01, 0, 0},
	{0x3a09, 0x50, 0, 0}, {0x3a0a, 0x01, 0, 0}, {0x3a0b, 0x18, 0, 0},
	{0x3a0e, 0x03, 0, 0}, {0x3a0d, 0x04, 0, 0}, {0x3a14, 0x04, 0, 0},
	{0x3a15, 0x60, 0, 0}, {0x4713, 0x02, 0, 0}, {0x4407, 0x04, 0, 0},
	{0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0}, {0x3824, 0x04, 0, 0},
	{0x4005, 0x1a, 0, 0}, {0x3008, 0x02, 0, 0}, {0x3503, 0, 0, 0},
};

static struct reg_value ov5640_setting_15fps_QSXGA_2592_1944[] = {
	{0x4202, 0x0f, 0, 0},	/* stream off the sensor */
	{0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0}, /*disable flip*/
	{0x3035, 0x21, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0}, {0x3814, 0x11, 0, 0},
	{0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x00, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9f, 0, 0},
	{0x3808, 0x0a, 0, 0}, {0x3809, 0x20, 0, 0}, {0x380a, 0x07, 0, 0},
	{0x380b, 0x98, 0, 0}, {0x380c, 0x0b, 0, 0}, {0x380d, 0x1c, 0, 0},
	{0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x04, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x21, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x06, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0x83, 0, 70},
	{0x4202, 0x00, 0, 0},	/* stream on the sensor */
};

static struct ov5640_mode_info ov5640_mode_info_data[2][ov5640_mode_MAX + 1] = {
	{
		{ov5640_mode_VGA_640_480,    640,  480,
		ov5640_setting_15fps_VGA_640_480,
		ARRAY_SIZE(ov5640_setting_15fps_VGA_640_480)},
		{ov5640_mode_QVGA_320_240,   320,  240,
		ov5640_setting_15fps_QVGA_320_240,
		ARRAY_SIZE(ov5640_setting_15fps_QVGA_320_240)},
		{ov5640_mode_NTSC_720_480,   720, 480,
		ov5640_setting_15fps_NTSC_720_480,
		ARRAY_SIZE(ov5640_setting_15fps_NTSC_720_480)},
		{ov5640_mode_PAL_720_576,    720, 576,
		ov5640_setting_15fps_PAL_720_576,
		ARRAY_SIZE(ov5640_setting_15fps_PAL_720_576)},
		{ov5640_mode_720P_1280_720,  1280, 720,
		ov5640_setting_15fps_720P_1280_720,
		ARRAY_SIZE(ov5640_setting_15fps_720P_1280_720)},
		{ov5640_mode_1080P_1920_1080,  1920, 1080,
		ov5640_setting_15fps_1080P_1920_1080,
		ARRAY_SIZE(ov5640_setting_15fps_1080P_1920_1080)},
		{ov5640_mode_QSXGA_2592_1944, 2592, 1944,
		ov5640_setting_15fps_QSXGA_2592_1944,
		ARRAY_SIZE(ov5640_setting_15fps_QSXGA_2592_1944)},
		{ov5640_mode_QCIF_176_144, 176, 144,
		ov5640_setting_15fps_QCIF_176_144,
		ARRAY_SIZE(ov5640_setting_15fps_QCIF_176_144)},
	},
	{
		{ov5640_mode_VGA_640_480,    640,  480,
		ov5640_setting_30fps_VGA_640_480,
		ARRAY_SIZE(ov5640_setting_30fps_VGA_640_480)},
		{ov5640_mode_QVGA_320_240,   320,  240,
		ov5640_setting_30fps_QVGA_320_240,
		ARRAY_SIZE(ov5640_setting_30fps_QVGA_320_240)},
		{ov5640_mode_NTSC_720_480,   720, 480,
		ov5640_setting_30fps_NTSC_720_480,
		ARRAY_SIZE(ov5640_setting_30fps_NTSC_720_480)},
		{ov5640_mode_PAL_720_576,    720, 576,
		ov5640_setting_30fps_PAL_720_576,
		ARRAY_SIZE(ov5640_setting_30fps_PAL_720_576)},
		{ov5640_mode_720P_1280_720,  1280, 720,
		ov5640_setting_30fps_720P_1280_720,
		ARRAY_SIZE(ov5640_setting_30fps_720P_1280_720)},
		{ov5640_mode_1080P_1920_1080,  1920, 1080,
		ov5640_setting_30fps_1080P_1920_1080,
		ARRAY_SIZE(ov5640_setting_30fps_1080P_1920_1080)},
		{ov5640_mode_QSXGA_2592_1944, 0, 0, NULL, 0},
		{ov5640_mode_QCIF_176_144, 176, 144,
		ov5640_setting_30fps_QCIF_176_144,
		ARRAY_SIZE(ov5640_setting_30fps_QCIF_176_144)},
	},
};

static struct regulator *io_regulator;
static struct regulator *core_regulator;
static struct regulator *analog_regulator;
static struct regulator *gpo_regulator;
static struct fsl_mxc_camera_platform_data *camera_plat;

static int ov5640_probe(struct i2c_client *adapter,
				const struct i2c_device_id *device_id);
static int ov5640_remove(struct i2c_client *client);

static s32 ov5640_read_reg(u16 reg, u8 *val);
static s32 ov5640_write_reg(u16 reg, u8 val);

static const struct i2c_device_id ov5640_id[] = {
	{"ov5640_mipi", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, ov5640_id);

static struct i2c_driver ov5640_i2c_driver = {
	.driver = {
		  .owner = THIS_MODULE,
		  .name  = "ov5640_mipi",
		  },
	.probe  = ov5640_probe,
	.remove = ov5640_remove,
	.id_table = ov5640_id,
};


static s32 ov5640_write_reg(u16 reg, u8 val)
{
	u8 au8Buf[3] = {0};

	au8Buf[0] = reg >> 8;
	au8Buf[1] = reg & 0xff;
	au8Buf[2] = val;

	if (i2c_master_send(ov5640_data.i2c_client, au8Buf, 3) < 0) {
		pr_err("%s:write reg error:reg=%x,val=%x\n",
			__func__, reg, val);
		return -1;
	}

	return 0;
}

static s32 ov5640_read_reg(u16 reg, u8 *val)
{
	u8 au8RegBuf[2] = {0};
	u8 u8RdVal = 0;

	au8RegBuf[0] = reg >> 8;
	au8RegBuf[1] = reg & 0xff;

	if (2 != i2c_master_send(ov5640_data.i2c_client, au8RegBuf, 2)) {
		pr_err("%s:write reg error:reg=%x\n",
				__func__, reg);
		return -1;
	}

	if (1 != i2c_master_recv(ov5640_data.i2c_client, &u8RdVal, 1)) {
		pr_err("%s:read reg error:reg=%x,val=%x\n",
				__func__, reg, u8RdVal);
		return -1;
	}

	*val = u8RdVal;

	return u8RdVal;
}

static int preview_sysclk, preview_HTS, preview_sysclk;
static int AE_low, AE_high, AE_Target = 52;
static int XVCLK = 2200;

void OV5640_stream_on(void)
{
	ov5640_write_reg(0x4202, 0x00);
}

void OV5640_stream_off(void)
{
	ov5640_write_reg(0x4202, 0x0f);
}


int OV5640_get_sysclk(void)
{
	 /* calculate sysclk */
	int temp1, temp2;
	int Multiplier, PreDiv, VCO, SysDiv, Pll_rdiv, Bit_div2x, sclk_rdiv, sysclk;
	u8 temp;

	int sclk_rdiv_map[] = {1, 2, 4, 8};

	temp1 = ov5640_read_reg(0x3034, &temp);
	temp2 = temp1 & 0x0f;
	if (temp2 == 8 || temp2 == 10) {
		Bit_div2x = temp2 / 2;
	}

	temp1 = ov5640_read_reg(0x3035, &temp);
	SysDiv = temp1>>4;
	if (SysDiv == 0) {
	       SysDiv = 16;
	}

	temp1 = ov5640_read_reg(0x3036, &temp);
	Multiplier = temp1;

	temp1 = ov5640_read_reg(0x3037, &temp);
	PreDiv = temp1 & 0x0f;
	Pll_rdiv = ((temp1 >> 4) & 0x01) + 1;

	temp1 = ov5640_read_reg(0x3108, &temp);
	temp2 = temp1 & 0x03;
	sclk_rdiv = sclk_rdiv_map[temp2];

	VCO = XVCLK * Multiplier / PreDiv;

	sysclk = VCO / SysDiv / Pll_rdiv * 2 / Bit_div2x / sclk_rdiv;

	return sysclk;
}

void OV5640_set_night_mode(void)
{
	 /* read HTS from register settings */
	u8 mode;

	ov5640_read_reg(0x3a00, &mode);
	mode &= 0xfb;
	ov5640_write_reg(0x3a00, mode);
}

int OV5640_get_HTS(void)
{
	 /* read HTS from register settings */
	int HTS;
	u8 temp;

	HTS = ov5640_read_reg(0x380c, &temp);
	HTS = (HTS<<8) + ov5640_read_reg(0x380d, &temp);

	return HTS;
}

int OV5640_get_VTS(void)
{
	 /* read VTS from register settings */
	int VTS;
	u8 temp;

	VTS = ov5640_read_reg(0x380e, &temp);/* total vertical size[15:8] high byte */

	VTS = (VTS<<8) + ov5640_read_reg(0x380f, &temp);

	return VTS;
}

int OV5640_set_VTS(int VTS)
{
	 /* write VTS to registers */
	 int temp;

	 temp = VTS & 0xff;
	 ov5640_write_reg(0x380f, temp);

	 temp = VTS>>8;
	 ov5640_write_reg(0x380e, temp);

	 return 0;
}

int OV5640_get_shutter(void)
{
	 /* read shutter, in number of line period */
	int shutter;
	u8 temp;

	shutter = (ov5640_read_reg(0x03500, &temp) & 0x0f);
	shutter = (shutter<<8) + ov5640_read_reg(0x3501, &temp);
	shutter = (shutter<<4) + (ov5640_read_reg(0x3502, &temp)>>4);

	 return shutter;
}

int OV5640_set_shutter(int shutter)
{
	 /* write shutter, in number of line period */
	 int temp;

	 shutter = shutter & 0xffff;

	 temp = shutter & 0x0f;
	 temp = temp<<4;
	 ov5640_write_reg(0x3502, temp);

	 temp = shutter & 0xfff;
	 temp = temp>>4;
	 ov5640_write_reg(0x3501, temp);

	 temp = shutter>>12;
	 ov5640_write_reg(0x3500, temp);

	 return 0;
}

int OV5640_get_gain16(void)
{
	 /* read gain, 16 = 1x */
	int gain16;
	u8 temp;

	gain16 = ov5640_read_reg(0x350a, &temp) & 0x03;
	gain16 = (gain16<<8) + ov5640_read_reg(0x350b, &temp);

	return gain16;
}

int OV5640_set_gain16(int gain16)
{
	/* write gain, 16 = 1x */
	u8 temp;
	gain16 = gain16 & 0x3ff;

	temp = gain16 & 0xff;
	ov5640_write_reg(0x350b, temp);

	temp = gain16>>8;
	ov5640_write_reg(0x350a, temp);

	return 0;
}

int OV5640_get_light_frequency(void)
{
	/* get banding filter value */
	int temp, temp1, light_frequency = 0;
	u8 tmp;

	temp = ov5640_read_reg(0x3c01, &tmp);

	if (temp & 0x80) {
		/* manual */
		temp1 = ov5640_read_reg(0x3c00, &tmp);
		if (temp1 & 0x04) {
			/* 50Hz */
			light_frequency = 50;
		} else {
			/* 60Hz */
			light_frequency = 60;
		}
	} else {
		/* auto */
		temp1 = ov5640_read_reg(0x3c0c, &tmp);
		if (temp1 & 0x01) {
			/* 50Hz */
			light_frequency = 50;
		} else {
			/* 60Hz */
		}
	}
	return light_frequency;
}

void OV5640_set_bandingfilter(void)
{
	int preview_VTS;
	int band_step60, max_band60, band_step50, max_band50;

	/* read preview PCLK */
	preview_sysclk = OV5640_get_sysclk();
	/* read preview HTS */
	preview_HTS = OV5640_get_HTS();

	/* read preview VTS */
	preview_VTS = OV5640_get_VTS();

	/* calculate banding filter */
	/* 60Hz */
	band_step60 = preview_sysclk * 100/preview_HTS * 100/120;
	ov5640_write_reg(0x3a0a, (band_step60 >> 8));
	ov5640_write_reg(0x3a0b, (band_step60 & 0xff));

	max_band60 = (int)((preview_VTS-4)/band_step60);
	ov5640_write_reg(0x3a0d, max_band60);

	/* 50Hz */
	band_step50 = preview_sysclk * 100/preview_HTS;
	ov5640_write_reg(0x3a08, (band_step50 >> 8));
	ov5640_write_reg(0x3a09, (band_step50 & 0xff));

	max_band50 = (int)((preview_VTS-4)/band_step50);
	ov5640_write_reg(0x3a0e, max_band50);
}

int OV5640_set_AE_target(int target)
{
	/* stable in high */
	int fast_high, fast_low;
	AE_low = target * 23 / 25;	/* 0.92 */
	AE_high = target * 27 / 25;	/* 1.08 */

	fast_high = AE_high<<1;
	if (fast_high > 255)
		fast_high = 255;

	fast_low = AE_low >> 1;

	ov5640_write_reg(0x3a0f, AE_high);
	ov5640_write_reg(0x3a10, AE_low);
	ov5640_write_reg(0x3a1b, AE_high);
	ov5640_write_reg(0x3a1e, AE_low);
	ov5640_write_reg(0x3a11, fast_high);
	ov5640_write_reg(0x3a1f, fast_low);

	return 0;
}

bool binning_on(void)
{
	u8 temp;
	ov5640_read_reg(0x3821, &temp);
	temp &= 0xfe;
	if (temp)
		return true;
	else
		return false;
}

static void ov5640_set_virtual_channel(int channel)
{
	u8 channel_id;

	ov5640_read_reg(0x4814, &channel_id);
	channel_id &= ~(3 << 6);
	ov5640_write_reg(0x4814, channel_id | (channel << 6));
}

static int ov5640_init_mode(enum ov5640_frame_rate frame_rate,
			    enum ov5640_mode mode)
{
	struct reg_value *pModeSetting = NULL;
		s32 i = 0;
	s32 iModeSettingArySize = 0;
	register u32 Delay_ms = 0;
	register u16 RegAddr = 0;
	register u8 Mask = 0;
	register u8 Val = 0;
	u8 RegVal = 0;
	int retval = 0;
	void *mipi_csi2_info;
	u32 mipi_reg;

	if ((mode > ov5640_mode_MAX || mode < ov5640_mode_MIN)
		&& (mode != ov5640_mode_INIT)) {
		pr_err("Wrong ov5640 mode detected!\n");
		return -1;
	}

	mipi_csi2_info = mipi_csi2_get_info();

	/* initial mipi dphy */
	if (mipi_csi2_info) {
		if (!mipi_csi2_get_status(mipi_csi2_info))
			mipi_csi2_enable(mipi_csi2_info);

		if (mipi_csi2_get_status(mipi_csi2_info)) {
			mipi_csi2_set_lanes(mipi_csi2_info);

			/*Only reset MIPI CSI2 HW at sensor initialize*/
			if (mode == ov5640_mode_INIT)
				mipi_csi2_reset(mipi_csi2_info);

			if (ov5640_data.pix.pixelformat == V4L2_PIX_FMT_UYVY)
				mipi_csi2_set_datatype(mipi_csi2_info, MIPI_DT_YUV422);
			else if (ov5640_data.pix.pixelformat == V4L2_PIX_FMT_RGB565)
				mipi_csi2_set_datatype(mipi_csi2_info, MIPI_DT_RGB565);
			else
				pr_err("currently this sensor format can not be supported!\n");
		} else {
			pr_err("Can not enable mipi csi2 driver!\n");
			return -1;
		}
	} else {
		printk(KERN_ERR "Fail to get mipi_csi2_info!\n");
		return -1;
	}

	if (mode == ov5640_mode_INIT) {
		pModeSetting = ov5640_init_setting_30fps_VGA;
		iModeSettingArySize = ARRAY_SIZE(ov5640_init_setting_30fps_VGA);

		ov5640_data.pix.width = 640;
		ov5640_data.pix.height = 480;

		for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
			Delay_ms = pModeSetting->u32Delay_ms;
			RegAddr = pModeSetting->u16RegAddr;
			Val = pModeSetting->u8Val;
			Mask = pModeSetting->u8Mask;

			if (Mask) {
				retval = ov5640_read_reg(RegAddr, &RegVal);
				if (retval < 0)
					goto err;

				RegVal &= ~(u8)Mask;
				Val &= Mask;
				Val |= RegVal;
			}

			retval = ov5640_write_reg(RegAddr, Val);
			if (retval < 0)
				goto err;

			if (Delay_ms)
				msleep(Delay_ms);
		}


		pModeSetting = ov5640_setting_30fps_VGA_640_480;
		iModeSettingArySize = ARRAY_SIZE(ov5640_setting_30fps_VGA_640_480);

		for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
			Delay_ms = pModeSetting->u32Delay_ms;
			RegAddr = pModeSetting->u16RegAddr;
			Val = pModeSetting->u8Val;
			Mask = pModeSetting->u8Mask;

			if (Mask) {
				retval = ov5640_read_reg(RegAddr, &RegVal);
				if (retval < 0)
					goto err;

				RegVal &= ~(u8)Mask;
				Val &= Mask;
				Val |= RegVal;
			}

			retval = ov5640_write_reg(RegAddr, Val);
			if (retval < 0)
				goto err;

			if (Delay_ms)
				msleep(Delay_ms);
		}
	} else {
		 /* set OV5640 to capture mode */

		u8 ae_ag_ctrl, average;
		int preview_shutter, preview_gain16;
		int capture_shutter, capture_gain16;
		int capture_sysclk, capture_HTS, capture_VTS;
		int light_frequency, capture_bandingfilter, capture_max_band;
		long capture_gain16_shutter;

		/* auto focus */
		/* OV5640_auto_focus();//if no af function, just skip it */

		/* turn off AE/AG */
		ov5640_read_reg(0x3503, &ae_ag_ctrl);
		ae_ag_ctrl = ae_ag_ctrl | 0x03;
		ov5640_write_reg(0x3503, ae_ag_ctrl);

		/* read preview shutter */

		preview_shutter = OV5640_get_shutter();
		if ((binning_on()) && (mode != ov5640_mode_720P_1280_720)
				&& (mode != ov5640_mode_1080P_1920_1080))
			preview_shutter *= 2;

		/* read preview gain */
		preview_gain16 = OV5640_get_gain16();

		/* get average */
		ov5640_read_reg(0x56a1, &average);

		/* turn off night mode for capture */
		OV5640_set_night_mode();

		/* turn off overlay */
		/* ov5640_write_reg(0x3022, 0x06);//if no af function, just skip it */

		OV5640_stream_off();

		/* Write capture setting */

		pModeSetting =
			ov5640_mode_info_data[frame_rate][mode].init_data_ptr;
		iModeSettingArySize =
			ov5640_mode_info_data[frame_rate][mode].init_data_size;

		ov5640_data.pix.width =
			ov5640_mode_info_data[frame_rate][mode].width;
		ov5640_data.pix.height =
			ov5640_mode_info_data[frame_rate][mode].height;

		if (ov5640_data.pix.width == 0 || ov5640_data.pix.height == 0 ||
		    pModeSetting == NULL || iModeSettingArySize == 0)
			return -EINVAL;

		for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
			Delay_ms = pModeSetting->u32Delay_ms;
			RegAddr = pModeSetting->u16RegAddr;
			Val = pModeSetting->u8Val;
			Mask = pModeSetting->u8Mask;

			if (Mask) {
				retval = ov5640_read_reg(RegAddr, &RegVal);
				if (retval < 0)
					goto err;

				RegVal &= ~(u8)Mask;
				Val &= Mask;
				Val |= RegVal;
			}

			retval = ov5640_write_reg(RegAddr, Val);
			if (retval < 0)
				goto err;

			if (Delay_ms)
				msleep(Delay_ms);
		}


		/* read capture VTS */
		capture_VTS = OV5640_get_VTS();
		capture_HTS = OV5640_get_HTS();
		capture_sysclk = OV5640_get_sysclk();

		/* calculate capture banding filter */
		light_frequency = OV5640_get_light_frequency();
		if (light_frequency == 60) {
			/* 60Hz */
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS * 100 / 120;
		} else {
			/* 50Hz */
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS;
		}
		capture_max_band = (int)((capture_VTS - 4)/capture_bandingfilter);

		/* calculate capture shutter/gain16 */
		if (average > AE_low && average < AE_high) {
			/* in stable range */
			capture_gain16_shutter = preview_gain16 * preview_shutter * capture_sysclk/preview_sysclk \
				* preview_HTS/capture_HTS * AE_Target / average;
		} else {
			capture_gain16_shutter = preview_gain16 * preview_shutter * capture_sysclk/preview_sysclk \
				* preview_HTS/capture_HTS;
		}

		/* gain to shutter */
		if (capture_gain16_shutter < (capture_bandingfilter * 16)) {
			/* shutter < 1/100 */
			capture_shutter = capture_gain16_shutter/16;
			if (capture_shutter < 1)
				capture_shutter = 1;

			capture_gain16 = capture_gain16_shutter/capture_shutter;
			if (capture_gain16 < 16)
				capture_gain16 = 16;
		} else {
			if (capture_gain16_shutter > (capture_bandingfilter*capture_max_band*16)) {
			 /* exposure reach max */
			 capture_shutter = capture_bandingfilter*capture_max_band;
			 capture_gain16 = capture_gain16_shutter / capture_shutter;
			} else {
			 /* 1/100 < capture_shutter =< max, capture_shutter = n/100 */
			 capture_shutter = ((int) (capture_gain16_shutter/16/capture_bandingfilter))\
				* capture_bandingfilter;
			 capture_gain16 = capture_gain16_shutter / capture_shutter;
			}
		}


		/* write capture gain */
		OV5640_set_gain16(capture_gain16);

		/* write capture shutter */
		if (capture_shutter > (capture_VTS - 4)) {
			capture_VTS = capture_shutter + 4;
			OV5640_set_VTS(capture_VTS);
		}
		OV5640_set_shutter(capture_shutter);

		OV5640_stream_on();

	}

	OV5640_set_AE_target(AE_Target);
	OV5640_get_light_frequency();
	OV5640_set_bandingfilter();
	ov5640_set_virtual_channel(ov5640_data.csi);

	if (mipi_csi2_info) {
		unsigned int i;

		i = 0;

		/* wait for mipi sensor ready */
		mipi_reg = mipi_csi2_dphy_status(mipi_csi2_info);
		while ((mipi_reg == 0x200) && (i < 10)) {
			mipi_reg = mipi_csi2_dphy_status(mipi_csi2_info);
			i++;
			msleep(10);
		}

		if (i >= 10) {
			pr_err("mipi csi2 can not receive sensor clk!\n");
			return -1;
		}

		i = 0;

		/* wait for mipi stable */
		mipi_reg = mipi_csi2_get_error1(mipi_csi2_info);
		while ((mipi_reg != 0x0) && (i < 10)) {
			mipi_reg = mipi_csi2_get_error1(mipi_csi2_info);
			i++;
			msleep(10);
		}

		if (i >= 10) {
			pr_err("mipi csi2 can not reveive data correctly!\n");
			return -1;
		}
	}
err:
	return retval;
}

/* --------------- IOCTL functions from v4l2_int_ioctl_desc --------------- */

static int ioctl_g_ifparm(struct v4l2_int_device *s, struct v4l2_ifparm *p)
{
	if (s == NULL) {
		pr_err("   ERROR!! no slave device set!\n");
		return -1;
	}

	memset(p, 0, sizeof(*p));
	p->u.bt656.clock_curr = ov5640_data.mclk;
	pr_debug("   clock_curr=mclk=%d\n", ov5640_data.mclk);
	p->if_type = V4L2_IF_TYPE_BT656;
	p->u.bt656.mode = V4L2_IF_TYPE_BT656_MODE_NOBT_8BIT;
	p->u.bt656.clock_min = OV5640_XCLK_MIN;
	p->u.bt656.clock_max = OV5640_XCLK_MAX;
	p->u.bt656.bt_sync_correct = 1;  /* Indicate external vsync */

	return 0;
}

/*!
 * ioctl_s_power - V4L2 sensor interface handler for VIDIOC_S_POWER ioctl
 * @s: pointer to standard V4L2 device structure
 * @on: indicates power mode (on or off)
 *
 * Turns the power on or off, depending on the value of on and returns the
 * appropriate error code.
 */
static int ioctl_s_power(struct v4l2_int_device *s, int on)
{
	struct sensor_data *sensor = s->priv;

	if (on && !sensor->on) {
		if (io_regulator)
			if (regulator_enable(io_regulator) != 0)
				return -EIO;
		if (core_regulator)
			if (regulator_enable(core_regulator) != 0)
				return -EIO;
		if (gpo_regulator)
			if (regulator_enable(gpo_regulator) != 0)
				return -EIO;
		if (analog_regulator)
			if (regulator_enable(analog_regulator) != 0)
				return -EIO;
		/* Make sure power on */
		if (camera_plat->pwdn)
			camera_plat->pwdn(0);

	} else if (!on && sensor->on) {
		if (analog_regulator)
			regulator_disable(analog_regulator);
		if (core_regulator)
			regulator_disable(core_regulator);
		if (io_regulator)
			regulator_disable(io_regulator);
		if (gpo_regulator)
			regulator_disable(gpo_regulator);

		if (camera_plat->pwdn)
			camera_plat->pwdn(1);
	}

	sensor->on = on;

	return 0;
}

/*!
 * ioctl_g_parm - V4L2 sensor interface handler for VIDIOC_G_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_G_PARM ioctl structure
 *
 * Returns the sensor's video CAPTURE parameters.
 */
static int ioctl_g_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct sensor_data *sensor = s->priv;
	struct v4l2_captureparm *cparm = &a->parm.capture;
	int ret = 0;

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		memset(a, 0, sizeof(*a));
		a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = sensor->streamcap.capability;
		cparm->timeperframe = sensor->streamcap.timeperframe;
		cparm->capturemode = sensor->streamcap.capturemode;
		ret = 0;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*!
 * ioctl_s_parm - V4L2 sensor interface handler for VIDIOC_S_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_S_PARM ioctl structure
 *
 * Configures the sensor to use the input parameters, if possible.  If
 * not possible, reverts to the old parameters and returns the
 * appropriate error code.
 */
static int ioctl_s_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct sensor_data *sensor = s->priv;
	struct v4l2_fract *timeperframe = &a->parm.capture.timeperframe;
	u32 tgt_fps;	/* target frames per secound */
	enum ov5640_frame_rate frame_rate;
	int ret = 0;

	/* Make sure power on */
	if (camera_plat->pwdn)
		camera_plat->pwdn(0);

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		/* Check that the new frame rate is allowed. */
		if ((timeperframe->numerator == 0) ||
		    (timeperframe->denominator == 0)) {
			timeperframe->denominator = DEFAULT_FPS;
			timeperframe->numerator = 1;
		}

		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps > MAX_FPS) {
			timeperframe->denominator = MAX_FPS;
			timeperframe->numerator = 1;
		} else if (tgt_fps < MIN_FPS) {
			timeperframe->denominator = MIN_FPS;
			timeperframe->numerator = 1;
		}

		/* Actual frame rate we use */
		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps == 15)
			frame_rate = ov5640_15_fps;
		else if (tgt_fps == 30)
			frame_rate = ov5640_30_fps;
		else {
			pr_err(" The camera frame rate is not supported!\n");
			return -EINVAL;
		}

		sensor->streamcap.timeperframe = *timeperframe;
		sensor->streamcap.capturemode =
				(u32)a->parm.capture.capturemode;

		ret = ov5640_init_mode(frame_rate,
				       sensor->streamcap.capturemode);
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		pr_debug("   type is not " \
			"V4L2_BUF_TYPE_VIDEO_CAPTURE but %d\n",
			a->type);
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*!
 * ioctl_g_fmt_cap - V4L2 sensor interface handler for ioctl_g_fmt_cap
 * @s: pointer to standard V4L2 device structure
 * @f: pointer to standard V4L2 v4l2_format structure
 *
 * Returns the sensor's current pixel format in the v4l2_format
 * parameter.
 */
static int ioctl_g_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct sensor_data *sensor = s->priv;

	f->fmt.pix = sensor->pix;

	return 0;
}

/*!
 * ioctl_g_ctrl - V4L2 sensor interface handler for VIDIOC_G_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_G_CTRL ioctl structure
 *
 * If the requested control is supported, returns the control's current
 * value from the video_control[] array.  Otherwise, returns -EINVAL
 * if the control is not supported.
 */
static int ioctl_g_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int ret = 0;

	switch (vc->id) {
	case V4L2_CID_BRIGHTNESS:
		vc->value = ov5640_data.brightness;
		break;
	case V4L2_CID_HUE:
		vc->value = ov5640_data.hue;
		break;
	case V4L2_CID_CONTRAST:
		vc->value = ov5640_data.contrast;
		break;
	case V4L2_CID_SATURATION:
		vc->value = ov5640_data.saturation;
		break;
	case V4L2_CID_RED_BALANCE:
		vc->value = ov5640_data.red;
		break;
	case V4L2_CID_BLUE_BALANCE:
		vc->value = ov5640_data.blue;
		break;
	case V4L2_CID_EXPOSURE:
		vc->value = ov5640_data.ae_mode;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

/*!
 * ioctl_s_ctrl - V4L2 sensor interface handler for VIDIOC_S_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_S_CTRL ioctl structure
 *
 * If the requested control is supported, sets the control's current
 * value in HW (and updates the video_control[] array).  Otherwise,
 * returns -EINVAL if the control is not supported.
 */
static int ioctl_s_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int retval = 0;

	pr_debug("In ov5640:ioctl_s_ctrl %d\n",
		 vc->id);

	switch (vc->id) {
	case V4L2_CID_BRIGHTNESS:
		break;
	case V4L2_CID_CONTRAST:
		break;
	case V4L2_CID_SATURATION:
		break;
	case V4L2_CID_HUE:
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		break;
	case V4L2_CID_DO_WHITE_BALANCE:
		break;
	case V4L2_CID_RED_BALANCE:
		break;
	case V4L2_CID_BLUE_BALANCE:
		break;
	case V4L2_CID_GAMMA:
		break;
	case V4L2_CID_EXPOSURE:
		break;
	case V4L2_CID_AUTOGAIN:
		break;
	case V4L2_CID_GAIN:
		break;
	case V4L2_CID_HFLIP:
		break;
	case V4L2_CID_VFLIP:
		break;
	default:
		retval = -EPERM;
		break;
	}

	return retval;
}

/*!
 * ioctl_enum_framesizes - V4L2 sensor interface handler for
 *			   VIDIOC_ENUM_FRAMESIZES ioctl
 * @s: pointer to standard V4L2 device structure
 * @fsize: standard V4L2 VIDIOC_ENUM_FRAMESIZES ioctl structure
 *
 * Return 0 if successful, otherwise -EINVAL.
 */
static int ioctl_enum_framesizes(struct v4l2_int_device *s,
				 struct v4l2_frmsizeenum *fsize)
{
	if (fsize->index > ov5640_mode_MAX)
		return -EINVAL;

	fsize->pixel_format = ov5640_data.pix.pixelformat;
	fsize->discrete.width =
			max(ov5640_mode_info_data[0][fsize->index].width,
			    ov5640_mode_info_data[1][fsize->index].width);
	fsize->discrete.height =
			max(ov5640_mode_info_data[0][fsize->index].height,
			    ov5640_mode_info_data[1][fsize->index].height);
	return 0;
}

/*!
 * ioctl_g_chip_ident - V4L2 sensor interface handler for
 *			VIDIOC_DBG_G_CHIP_IDENT ioctl
 * @s: pointer to standard V4L2 device structure
 * @id: pointer to int
 *
 * Return 0.
 */
static int ioctl_g_chip_ident(struct v4l2_int_device *s, int *id)
{
	((struct v4l2_dbg_chip_ident *)id)->match.type =
					V4L2_CHIP_MATCH_I2C_DRIVER;
	strcpy(((struct v4l2_dbg_chip_ident *)id)->match.name, "ov5640_camera");

	return 0;
}

/*!
 * ioctl_init - V4L2 sensor interface handler for VIDIOC_INT_INIT
 * @s: pointer to standard V4L2 device structure
 */
static int ioctl_init(struct v4l2_int_device *s)
{

	return 0;
}

/*!
 * ioctl_enum_fmt_cap - V4L2 sensor interface handler for VIDIOC_ENUM_FMT
 * @s: pointer to standard V4L2 device structure
 * @fmt: pointer to standard V4L2 fmt description structure
 *
 * Return 0.
 */
static int ioctl_enum_fmt_cap(struct v4l2_int_device *s,
			      struct v4l2_fmtdesc *fmt)
{
	if (fmt->index > ov5640_mode_MAX)
		return -EINVAL;

	fmt->pixelformat = ov5640_data.pix.pixelformat;

	return 0;
}

/*!
 * ioctl_dev_init - V4L2 sensor interface handler for vidioc_int_dev_init_num
 * @s: pointer to standard V4L2 device structure
 *
 * Initialise the device when slave attaches to the master.
 */
static int ioctl_dev_init(struct v4l2_int_device *s)
{
	struct sensor_data *sensor = s->priv;
	u32 tgt_xclk;	/* target xclk */
	u32 tgt_fps;	/* target frames per secound */
	int ret;
	enum ov5640_frame_rate frame_rate;
	void *mipi_csi2_info;

	ov5640_data.on = true;

	/* mclk */
	tgt_xclk = ov5640_data.mclk;
	tgt_xclk = min(tgt_xclk, (u32)OV5640_XCLK_MAX);
	tgt_xclk = max(tgt_xclk, (u32)OV5640_XCLK_MIN);
	ov5640_data.mclk = tgt_xclk;

	pr_debug("   Setting mclk to %d MHz\n", tgt_xclk / 1000000);
	set_mclk_rate(&ov5640_data.mclk, ov5640_data.mclk_source);

	/* Default camera frame rate is set in probe */
	tgt_fps = sensor->streamcap.timeperframe.denominator /
		  sensor->streamcap.timeperframe.numerator;

	if (tgt_fps == 15)
		frame_rate = ov5640_15_fps;
	else if (tgt_fps == 30)
		frame_rate = ov5640_30_fps;
	else
		return -EINVAL; /* Only support 15fps or 30fps now. */

	mipi_csi2_info = mipi_csi2_get_info();

	/* enable mipi csi2 */
	if (mipi_csi2_info)
		mipi_csi2_enable(mipi_csi2_info);
	else {
		printk(KERN_ERR "Fail to get mipi_csi2_info!\n");
		return -EPERM;
	}

	ret = ov5640_init_mode(frame_rate, ov5640_mode_INIT);

	return ret;
}

/*!
 * ioctl_dev_exit - V4L2 sensor interface handler for vidioc_int_dev_exit_num
 * @s: pointer to standard V4L2 device structure
 *
 * Delinitialise the device when slave detaches to the master.
 */
static int ioctl_dev_exit(struct v4l2_int_device *s)
{
	void *mipi_csi2_info;

	mipi_csi2_info = mipi_csi2_get_info();

	/* disable mipi csi2 */
	if (mipi_csi2_info)
		if (mipi_csi2_get_status(mipi_csi2_info))
			mipi_csi2_disable(mipi_csi2_info);

	return 0;
}

/*!
 * This structure defines all the ioctls for this module and links them to the
 * enumeration.
 */
static struct v4l2_int_ioctl_desc ov5640_ioctl_desc[] = {
	{vidioc_int_dev_init_num, (v4l2_int_ioctl_func*) ioctl_dev_init},
	{vidioc_int_dev_exit_num, ioctl_dev_exit},
	{vidioc_int_s_power_num, (v4l2_int_ioctl_func*) ioctl_s_power},
	{vidioc_int_g_ifparm_num, (v4l2_int_ioctl_func*) ioctl_g_ifparm},
/*	{vidioc_int_g_needs_reset_num,
				(v4l2_int_ioctl_func *)ioctl_g_needs_reset}, */
/*	{vidioc_int_reset_num, (v4l2_int_ioctl_func *)ioctl_reset}, */
	{vidioc_int_init_num, (v4l2_int_ioctl_func*) ioctl_init},
	{vidioc_int_enum_fmt_cap_num,
				(v4l2_int_ioctl_func *) ioctl_enum_fmt_cap},
/*	{vidioc_int_try_fmt_cap_num,
				(v4l2_int_ioctl_func *)ioctl_try_fmt_cap}, */
	{vidioc_int_g_fmt_cap_num, (v4l2_int_ioctl_func *) ioctl_g_fmt_cap},
/*	{vidioc_int_s_fmt_cap_num, (v4l2_int_ioctl_func *) ioctl_s_fmt_cap}, */
	{vidioc_int_g_parm_num, (v4l2_int_ioctl_func *) ioctl_g_parm},
	{vidioc_int_s_parm_num, (v4l2_int_ioctl_func *) ioctl_s_parm},
/*	{vidioc_int_queryctrl_num, (v4l2_int_ioctl_func *)ioctl_queryctrl}, */
	{vidioc_int_g_ctrl_num, (v4l2_int_ioctl_func *) ioctl_g_ctrl},
	{vidioc_int_s_ctrl_num, (v4l2_int_ioctl_func *) ioctl_s_ctrl},
	{vidioc_int_enum_framesizes_num,
				(v4l2_int_ioctl_func *) ioctl_enum_framesizes},
	{vidioc_int_g_chip_ident_num,
				(v4l2_int_ioctl_func *) ioctl_g_chip_ident},
};

static struct v4l2_int_slave ov5640_slave = {
	.ioctls = ov5640_ioctl_desc,
	.num_ioctls = ARRAY_SIZE(ov5640_ioctl_desc),
};

static struct v4l2_int_device ov5640_int_device = {
	.module = THIS_MODULE,
	.name = "ov5640",
	.type = v4l2_int_type_slave,
	.u = {
		.slave = &ov5640_slave,
	},
};

/*!
 * ov5640 I2C probe function
 *
 * @param adapter            struct i2c_adapter *
 * @return  Error code indicating success or failure
 */
static int ov5640_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int retval;
	struct fsl_mxc_camera_platform_data *plat_data = client->dev.platform_data;

	/* Set initial values for the sensor struct. */
	memset(&ov5640_data, 0, sizeof(ov5640_data));
	ov5640_data.mclk = 24000000; /* 6 - 54 MHz, typical 24MHz */
	ov5640_data.mclk = plat_data->mclk;
	ov5640_data.mclk_source = plat_data->mclk_source;
	ov5640_data.csi = plat_data->csi;
	ov5640_data.io_init = plat_data->io_init;

	ov5640_data.i2c_client = client;
	ov5640_data.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	ov5640_data.pix.width = 640;
	ov5640_data.pix.height = 480;
	ov5640_data.streamcap.capability = V4L2_MODE_HIGHQUALITY |
					   V4L2_CAP_TIMEPERFRAME;
	ov5640_data.streamcap.capturemode = 0;
	ov5640_data.streamcap.timeperframe.denominator = DEFAULT_FPS;
	ov5640_data.streamcap.timeperframe.numerator = 1;

	if (plat_data->io_regulator) {
		io_regulator = regulator_get(&client->dev,
					     plat_data->io_regulator);
		if (!IS_ERR(io_regulator)) {
			regulator_set_voltage(io_regulator,
					      OV5640_VOLTAGE_DIGITAL_IO,
					      OV5640_VOLTAGE_DIGITAL_IO);
			if (regulator_enable(io_regulator) != 0) {
				pr_err("%s:io set voltage error\n", __func__);
				goto err1;
			} else {
				dev_dbg(&client->dev,
					"%s:io set voltage ok\n", __func__);
			}
		} else
			io_regulator = NULL;
	}

	if (plat_data->core_regulator) {
		core_regulator = regulator_get(&client->dev,
					       plat_data->core_regulator);
		if (!IS_ERR(core_regulator)) {
			regulator_set_voltage(core_regulator,
					      OV5640_VOLTAGE_DIGITAL_CORE,
					      OV5640_VOLTAGE_DIGITAL_CORE);
			if (regulator_enable(core_regulator) != 0) {
				pr_err("%s:core set voltage error\n", __func__);
				goto err2;
			} else {
				dev_dbg(&client->dev,
					"%s:core set voltage ok\n", __func__);
			}
		} else
			core_regulator = NULL;
	}

	if (plat_data->analog_regulator) {
		analog_regulator = regulator_get(&client->dev,
						 plat_data->analog_regulator);
		if (!IS_ERR(analog_regulator)) {
			regulator_set_voltage(analog_regulator,
					      OV5640_VOLTAGE_ANALOG,
					      OV5640_VOLTAGE_ANALOG);
			if (regulator_enable(analog_regulator) != 0) {
				pr_err("%s:analog set voltage error\n",
					__func__);
				goto err3;
			} else {
				dev_dbg(&client->dev,
					"%s:analog set voltage ok\n", __func__);
			}
		} else
			analog_regulator = NULL;
	}

	if (plat_data->io_init)
		plat_data->io_init();

	camera_plat = plat_data;

	ov5640_int_device.priv = &ov5640_data;
	retval = v4l2_int_device_register(&ov5640_int_device);

	return retval;

err3:
	if (core_regulator) {
		regulator_disable(core_regulator);
		regulator_put(core_regulator);
	}
err2:
	if (io_regulator) {
		regulator_disable(io_regulator);
		regulator_put(io_regulator);
	}
err1:
	return -1;
}

/*!
 * ov5640 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int ov5640_remove(struct i2c_client *client)
{
	v4l2_int_device_unregister(&ov5640_int_device);

	if (gpo_regulator) {
		regulator_disable(gpo_regulator);
		regulator_put(gpo_regulator);
	}

	if (analog_regulator) {
		regulator_disable(analog_regulator);
		regulator_put(analog_regulator);
	}

	if (core_regulator) {
		regulator_disable(core_regulator);
		regulator_put(core_regulator);
	}

	if (io_regulator) {
		regulator_disable(io_regulator);
		regulator_put(io_regulator);
	}

	return 0;
}

/*!
 * ov5640 init function
 * Called by insmod ov5640_camera.ko.
 *
 * @return  Error code indicating success or failure
 */
static __init int ov5640_init(void)
{
	u8 err;

	err = i2c_add_driver(&ov5640_i2c_driver);
	if (err != 0)
		pr_err("%s:driver registration failed, error=%d\n",
			__func__, err);

	return err;
}

/*!
 * OV5640 cleanup function
 * Called on rmmod ov5640_camera.ko
 *
 * @return  Error code indicating success or failure
 */
static void __exit ov5640_clean(void)
{
	i2c_del_driver(&ov5640_i2c_driver);
}

module_init(ov5640_init);
module_exit(ov5640_clean);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("OV5640 MIPI Camera Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_ALIAS("CSI");
