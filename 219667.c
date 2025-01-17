static void ov518_configure(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	/* For 518 and 518+ */
	static const struct ov_regvals init_518[] = {
		{ R51x_SYS_RESET,	0x40 },
		{ R51x_SYS_INIT,	0xe1 },
		{ R51x_SYS_RESET,	0x3e },
		{ R51x_SYS_INIT,	0xe1 },
		{ R51x_SYS_RESET,	0x00 },
		{ R51x_SYS_INIT,	0xe1 },
		{ 0x46,			0x00 },
		{ 0x5d,			0x03 },
	};

	static const struct ov_regvals norm_518[] = {
		{ R51x_SYS_SNAP,	0x02 }, /* Reset */
		{ R51x_SYS_SNAP,	0x01 }, /* Enable */
		{ 0x31,			0x0f },
		{ 0x5d,			0x03 },
		{ 0x24,			0x9f },
		{ 0x25,			0x90 },
		{ 0x20,			0x00 },
		{ 0x51,			0x04 },
		{ 0x71,			0x19 },
		{ 0x2f,			0x80 },
	};

	static const struct ov_regvals norm_518_p[] = {
		{ R51x_SYS_SNAP,	0x02 }, /* Reset */
		{ R51x_SYS_SNAP,	0x01 }, /* Enable */
		{ 0x31,			0x0f },
		{ 0x5d,			0x03 },
		{ 0x24,			0x9f },
		{ 0x25,			0x90 },
		{ 0x20,			0x60 },
		{ 0x51,			0x02 },
		{ 0x71,			0x19 },
		{ 0x40,			0xff },
		{ 0x41,			0x42 },
		{ 0x46,			0x00 },
		{ 0x33,			0x04 },
		{ 0x21,			0x19 },
		{ 0x3f,			0x10 },
		{ 0x2f,			0x80 },
	};

	/* First 5 bits of custom ID reg are a revision ID on OV518 */
	sd->revision = reg_r(sd, R51x_SYS_CUST_ID) & 0x1f;
	gspca_dbg(gspca_dev, D_PROBE, "Device revision %d\n", sd->revision);

	write_regvals(sd, init_518, ARRAY_SIZE(init_518));

	/* Set LED GPIO pin to output mode */
	reg_w_mask(sd, R518_GPIO_CTL, 0x00, 0x02);

	switch (sd->bridge) {
	case BRIDGE_OV518:
		write_regvals(sd, norm_518, ARRAY_SIZE(norm_518));
		break;
	case BRIDGE_OV518PLUS:
		write_regvals(sd, norm_518_p, ARRAY_SIZE(norm_518_p));
		break;
	}

	ov51x_upload_quan_tables(sd);

	reg_w(sd, 0x2f, 0x80);
}