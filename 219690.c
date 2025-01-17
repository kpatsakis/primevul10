static void ov519_set_mode(struct sd *sd)
{
	static const struct ov_regvals bridge_ov7660[2][10] = {
		{{0x10, 0x14}, {0x11, 0x1e}, {0x12, 0x00}, {0x13, 0x00},
		 {0x14, 0x00}, {0x15, 0x00}, {0x16, 0x00}, {0x20, 0x0c},
		 {0x25, 0x01}, {0x26, 0x00}},
		{{0x10, 0x28}, {0x11, 0x3c}, {0x12, 0x00}, {0x13, 0x00},
		 {0x14, 0x00}, {0x15, 0x00}, {0x16, 0x00}, {0x20, 0x0c},
		 {0x25, 0x03}, {0x26, 0x00}}
	};
	static const struct ov_i2c_regvals sensor_ov7660[2][3] = {
		{{0x12, 0x00}, {0x24, 0x00}, {0x0c, 0x0c}},
		{{0x12, 0x00}, {0x04, 0x00}, {0x0c, 0x00}}
	};
	static const struct ov_i2c_regvals sensor_ov7660_2[] = {
		{OV7670_R17_HSTART, 0x13},
		{OV7670_R18_HSTOP, 0x01},
		{OV7670_R32_HREF, 0x92},
		{OV7670_R19_VSTART, 0x02},
		{OV7670_R1A_VSTOP, 0x7a},
		{OV7670_R03_VREF, 0x00},
/*		{0x33, 0x00}, */
/*		{0x34, 0x07}, */
/*		{0x36, 0x00}, */
/*		{0x6b, 0x0a}, */
	};

	write_regvals(sd, bridge_ov7660[sd->gspca_dev.curr_mode],
			ARRAY_SIZE(bridge_ov7660[0]));
	write_i2c_regvals(sd, sensor_ov7660[sd->gspca_dev.curr_mode],
			ARRAY_SIZE(sensor_ov7660[0]));
	write_i2c_regvals(sd, sensor_ov7660_2,
			ARRAY_SIZE(sensor_ov7660_2));
}