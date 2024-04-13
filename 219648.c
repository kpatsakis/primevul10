static void mode_init_ov_sensor_regs(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int qvga, xstart, xend, ystart, yend;
	u8 v;

	qvga = gspca_dev->cam.cam_mode[gspca_dev->curr_mode].priv & 1;

	/******** Mode (VGA/QVGA) and sensor specific regs ********/
	switch (sd->sensor) {
	case SEN_OV2610:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x28, qvga ? 0x00 : 0x20, 0x20);
		i2c_w(sd, 0x24, qvga ? 0x20 : 0x3a);
		i2c_w(sd, 0x25, qvga ? 0x30 : 0x60);
		i2c_w_mask(sd, 0x2d, qvga ? 0x40 : 0x00, 0x40);
		i2c_w_mask(sd, 0x67, qvga ? 0xf0 : 0x90, 0xf0);
		i2c_w_mask(sd, 0x74, qvga ? 0x20 : 0x00, 0x20);
		return;
	case SEN_OV2610AE: {
		u8 v;

		/* frame rates:
		 *	10fps / 5 fps for 1600x1200
		 *	40fps / 20fps for 800x600
		 */
		v = 80;
		if (qvga) {
			if (sd->frame_rate < 25)
				v = 0x81;
		} else {
			if (sd->frame_rate < 10)
				v = 0x81;
		}
		i2c_w(sd, 0x11, v);
		i2c_w(sd, 0x12, qvga ? 0x60 : 0x20);
		return;
	    }
	case SEN_OV3610:
		if (qvga) {
			xstart = (1040 - gspca_dev->pixfmt.width) / 2 +
				(0x1f << 4);
			ystart = (776 - gspca_dev->pixfmt.height) / 2;
		} else {
			xstart = (2076 - gspca_dev->pixfmt.width) / 2 +
				(0x10 << 4);
			ystart = (1544 - gspca_dev->pixfmt.height) / 2;
		}
		xend = xstart + gspca_dev->pixfmt.width;
		yend = ystart + gspca_dev->pixfmt.height;
		/* Writing to the COMH register resets the other windowing regs
		   to their default values, so we must do this first. */
		i2c_w_mask(sd, 0x12, qvga ? 0x40 : 0x00, 0xf0);
		i2c_w_mask(sd, 0x32,
			   (((xend >> 1) & 7) << 3) | ((xstart >> 1) & 7),
			   0x3f);
		i2c_w_mask(sd, 0x03,
			   (((yend >> 1) & 3) << 2) | ((ystart >> 1) & 3),
			   0x0f);
		i2c_w(sd, 0x17, xstart >> 4);
		i2c_w(sd, 0x18, xend >> 4);
		i2c_w(sd, 0x19, ystart >> 3);
		i2c_w(sd, 0x1a, yend >> 3);
		return;
	case SEN_OV8610:
		/* For OV8610 qvga means qsvga */
		i2c_w_mask(sd, OV7610_REG_COM_C, qvga ? (1 << 5) : 0, 1 << 5);
		i2c_w_mask(sd, 0x13, 0x00, 0x20); /* Select 16 bit data bus */
		i2c_w_mask(sd, 0x12, 0x04, 0x06); /* AWB: 1 Test pattern: 0 */
		i2c_w_mask(sd, 0x2d, 0x00, 0x40); /* from windrv 090403 */
		i2c_w_mask(sd, 0x28, 0x20, 0x20); /* progressive mode on */
		break;
	case SEN_OV7610:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w(sd, 0x35, qvga ? 0x1e : 0x9e);
		i2c_w_mask(sd, 0x13, 0x00, 0x20); /* Select 16 bit data bus */
		i2c_w_mask(sd, 0x12, 0x04, 0x06); /* AWB: 1 Test pattern: 0 */
		break;
	case SEN_OV7620:
	case SEN_OV7620AE:
	case SEN_OV76BE:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x28, qvga ? 0x00 : 0x20, 0x20);
		i2c_w(sd, 0x24, qvga ? 0x20 : 0x3a);
		i2c_w(sd, 0x25, qvga ? 0x30 : 0x60);
		i2c_w_mask(sd, 0x2d, qvga ? 0x40 : 0x00, 0x40);
		i2c_w_mask(sd, 0x67, qvga ? 0xb0 : 0x90, 0xf0);
		i2c_w_mask(sd, 0x74, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x13, 0x00, 0x20); /* Select 16 bit data bus */
		i2c_w_mask(sd, 0x12, 0x04, 0x06); /* AWB: 1 Test pattern: 0 */
		if (sd->sensor == SEN_OV76BE)
			i2c_w(sd, 0x35, qvga ? 0x1e : 0x9e);
		break;
	case SEN_OV7640:
	case SEN_OV7648:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x28, qvga ? 0x00 : 0x20, 0x20);
		/* Setting this undocumented bit in qvga mode removes a very
		   annoying vertical shaking of the image */
		i2c_w_mask(sd, 0x2d, qvga ? 0x40 : 0x00, 0x40);
		/* Unknown */
		i2c_w_mask(sd, 0x67, qvga ? 0xf0 : 0x90, 0xf0);
		/* Allow higher automatic gain (to allow higher framerates) */
		i2c_w_mask(sd, 0x74, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x12, 0x04, 0x04); /* AWB: 1 */
		break;
	case SEN_OV7670:
		/* set COM7_FMT_VGA or COM7_FMT_QVGA
		 * do we need to set anything else?
		 *	HSTART etc are set in set_ov_sensor_window itself */
		i2c_w_mask(sd, OV7670_R12_COM7,
			 qvga ? OV7670_COM7_FMT_QVGA : OV7670_COM7_FMT_VGA,
			 OV7670_COM7_FMT_MASK);
		i2c_w_mask(sd, 0x13, 0x00, 0x20); /* Select 16 bit data bus */
		i2c_w_mask(sd, OV7670_R13_COM8, OV7670_COM8_AWB,
				OV7670_COM8_AWB);
		if (qvga) {		/* QVGA from ov7670.c by
					 * Jonathan Corbet */
			xstart = 164;
			xend = 28;
			ystart = 14;
			yend = 494;
		} else {		/* VGA */
			xstart = 158;
			xend = 14;
			ystart = 10;
			yend = 490;
		}
		/* OV7670 hardware window registers are split across
		 * multiple locations */
		i2c_w(sd, OV7670_R17_HSTART, xstart >> 3);
		i2c_w(sd, OV7670_R18_HSTOP, xend >> 3);
		v = i2c_r(sd, OV7670_R32_HREF);
		v = (v & 0xc0) | ((xend & 0x7) << 3) | (xstart & 0x07);
		msleep(10);	/* need to sleep between read and write to
				 * same reg! */
		i2c_w(sd, OV7670_R32_HREF, v);

		i2c_w(sd, OV7670_R19_VSTART, ystart >> 2);
		i2c_w(sd, OV7670_R1A_VSTOP, yend >> 2);
		v = i2c_r(sd, OV7670_R03_VREF);
		v = (v & 0xc0) | ((yend & 0x3) << 2) | (ystart & 0x03);
		msleep(10);	/* need to sleep between read and write to
				 * same reg! */
		i2c_w(sd, OV7670_R03_VREF, v);
		break;
	case SEN_OV6620:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x13, 0x00, 0x20); /* Select 16 bit data bus */
		i2c_w_mask(sd, 0x12, 0x04, 0x06); /* AWB: 1 Test pattern: 0 */
		break;
	case SEN_OV6630:
	case SEN_OV66308AF:
		i2c_w_mask(sd, 0x14, qvga ? 0x20 : 0x00, 0x20);
		i2c_w_mask(sd, 0x12, 0x04, 0x06); /* AWB: 1 Test pattern: 0 */
		break;
	case SEN_OV9600: {
		const struct ov_i2c_regvals *vals;
		static const struct ov_i2c_regvals sxga_15[] = {
			{0x11, 0x80}, {0x14, 0x3e}, {0x24, 0x85}, {0x25, 0x75}
		};
		static const struct ov_i2c_regvals sxga_7_5[] = {
			{0x11, 0x81}, {0x14, 0x3e}, {0x24, 0x85}, {0x25, 0x75}
		};
		static const struct ov_i2c_regvals vga_30[] = {
			{0x11, 0x81}, {0x14, 0x7e}, {0x24, 0x70}, {0x25, 0x60}
		};
		static const struct ov_i2c_regvals vga_15[] = {
			{0x11, 0x83}, {0x14, 0x3e}, {0x24, 0x80}, {0x25, 0x70}
		};

		/* frame rates:
		 *	15fps / 7.5 fps for 1280x1024
		 *	30fps / 15fps for 640x480
		 */
		i2c_w_mask(sd, 0x12, qvga ? 0x40 : 0x00, 0x40);
		if (qvga)
			vals = sd->frame_rate < 30 ? vga_15 : vga_30;
		else
			vals = sd->frame_rate < 15 ? sxga_7_5 : sxga_15;
		write_i2c_regvals(sd, vals, ARRAY_SIZE(sxga_15));
		return;
	    }
	default:
		return;
	}

	/******** Clock programming ********/
	i2c_w(sd, 0x11, sd->clockdiv);
}