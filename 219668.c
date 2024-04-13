static int sd_init(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam = &gspca_dev->cam;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		ov511_configure(gspca_dev);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		ov518_configure(gspca_dev);
		break;
	case BRIDGE_OV519:
		ov519_configure(sd);
		break;
	case BRIDGE_OVFX2:
		ovfx2_configure(sd);
		break;
	case BRIDGE_W9968CF:
		w9968cf_configure(sd);
		break;
	}

	/* The OV519 must be more aggressive about sensor detection since
	 * I2C write will never fail if the sensor is not present. We have
	 * to try to initialize the sensor to detect its presence */
	sd->sensor = -1;

	/* Test for 76xx */
	if (init_ov_sensor(sd, OV7xx0_SID) >= 0) {
		ov7xx0_configure(sd);

	/* Test for 6xx0 */
	} else if (init_ov_sensor(sd, OV6xx0_SID) >= 0) {
		ov6xx0_configure(sd);

	/* Test for 8xx0 */
	} else if (init_ov_sensor(sd, OV8xx0_SID) >= 0) {
		ov8xx0_configure(sd);

	/* Test for 3xxx / 2xxx */
	} else if (init_ov_sensor(sd, OV_HIRES_SID) >= 0) {
		ov_hires_configure(sd);
	} else {
		gspca_err(gspca_dev, "Can't determine sensor slave IDs\n");
		goto error;
	}

	if (sd->sensor < 0)
		goto error;

	ov51x_led_control(sd, 0);	/* turn LED off */

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		if (sd->sif) {
			cam->cam_mode = ov511_sif_mode;
			cam->nmodes = ARRAY_SIZE(ov511_sif_mode);
		}
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		if (sd->sif) {
			cam->cam_mode = ov518_sif_mode;
			cam->nmodes = ARRAY_SIZE(ov518_sif_mode);
		}
		break;
	case BRIDGE_OV519:
		if (sd->sif) {
			cam->cam_mode = ov519_sif_mode;
			cam->nmodes = ARRAY_SIZE(ov519_sif_mode);
		}
		break;
	case BRIDGE_OVFX2:
		switch (sd->sensor) {
		case SEN_OV2610:
		case SEN_OV2610AE:
			cam->cam_mode = ovfx2_ov2610_mode;
			cam->nmodes = ARRAY_SIZE(ovfx2_ov2610_mode);
			break;
		case SEN_OV3610:
			cam->cam_mode = ovfx2_ov3610_mode;
			cam->nmodes = ARRAY_SIZE(ovfx2_ov3610_mode);
			break;
		case SEN_OV9600:
			cam->cam_mode = ovfx2_ov9600_mode;
			cam->nmodes = ARRAY_SIZE(ovfx2_ov9600_mode);
			break;
		default:
			if (sd->sif) {
				cam->cam_mode = ov519_sif_mode;
				cam->nmodes = ARRAY_SIZE(ov519_sif_mode);
			}
			break;
		}
		break;
	case BRIDGE_W9968CF:
		if (sd->sif)
			cam->nmodes = ARRAY_SIZE(w9968cf_vga_mode) - 1;

		/* w9968cf needs initialisation once the sensor is known */
		w9968cf_init(sd);
		break;
	}

	/* initialize the sensor */
	switch (sd->sensor) {
	case SEN_OV2610:
		write_i2c_regvals(sd, norm_2610, ARRAY_SIZE(norm_2610));

		/* Enable autogain, autoexpo, awb, bandfilter */
		i2c_w_mask(sd, 0x13, 0x27, 0x27);
		break;
	case SEN_OV2610AE:
		write_i2c_regvals(sd, norm_2610ae, ARRAY_SIZE(norm_2610ae));

		/* enable autoexpo */
		i2c_w_mask(sd, 0x13, 0x05, 0x05);
		break;
	case SEN_OV3610:
		write_i2c_regvals(sd, norm_3620b, ARRAY_SIZE(norm_3620b));

		/* Enable autogain, autoexpo, awb, bandfilter */
		i2c_w_mask(sd, 0x13, 0x27, 0x27);
		break;
	case SEN_OV6620:
		write_i2c_regvals(sd, norm_6x20, ARRAY_SIZE(norm_6x20));
		break;
	case SEN_OV6630:
	case SEN_OV66308AF:
		write_i2c_regvals(sd, norm_6x30, ARRAY_SIZE(norm_6x30));
		break;
	default:
/*	case SEN_OV7610: */
/*	case SEN_OV76BE: */
		write_i2c_regvals(sd, norm_7610, ARRAY_SIZE(norm_7610));
		i2c_w_mask(sd, 0x0e, 0x00, 0x40);
		break;
	case SEN_OV7620:
	case SEN_OV7620AE:
		write_i2c_regvals(sd, norm_7620, ARRAY_SIZE(norm_7620));
		break;
	case SEN_OV7640:
	case SEN_OV7648:
		write_i2c_regvals(sd, norm_7640, ARRAY_SIZE(norm_7640));
		break;
	case SEN_OV7660:
		i2c_w(sd, OV7670_R12_COM7, OV7670_COM7_RESET);
		msleep(14);
		reg_w(sd, OV519_R57_SNAPSHOT, 0x23);
		write_regvals(sd, init_519_ov7660,
				ARRAY_SIZE(init_519_ov7660));
		write_i2c_regvals(sd, norm_7660, ARRAY_SIZE(norm_7660));
		sd->gspca_dev.curr_mode = 1;	/* 640x480 */
		ov519_set_mode(sd);
		ov519_set_fr(sd);
		sd_reset_snapshot(gspca_dev);
		ov51x_restart(sd);
		ov51x_stop(sd);			/* not in win traces */
		ov51x_led_control(sd, 0);
		break;
	case SEN_OV7670:
		write_i2c_regvals(sd, norm_7670, ARRAY_SIZE(norm_7670));
		break;
	case SEN_OV8610:
		write_i2c_regvals(sd, norm_8610, ARRAY_SIZE(norm_8610));
		break;
	case SEN_OV9600:
		write_i2c_regvals(sd, norm_9600, ARRAY_SIZE(norm_9600));

		/* enable autoexpo */
/*		i2c_w_mask(sd, 0x13, 0x05, 0x05); */
		break;
	}
	return gspca_dev->usb_err;
error:
	gspca_err(gspca_dev, "OV519 Config failed\n");
	return -EINVAL;
}