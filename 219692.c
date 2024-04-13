static int sd_start(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	/* Default for most bridges, allow bridge_mode_init_regs to override */
	sd->sensor_width = sd->gspca_dev.pixfmt.width;
	sd->sensor_height = sd->gspca_dev.pixfmt.height;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		ov511_mode_init_regs(sd);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		ov518_mode_init_regs(sd);
		break;
	case BRIDGE_OV519:
		ov519_mode_init_regs(sd);
		break;
	/* case BRIDGE_OVFX2: nothing to do */
	case BRIDGE_W9968CF:
		w9968cf_mode_init_regs(sd);
		break;
	}

	set_ov_sensor_window(sd);

	/* Force clear snapshot state in case the snapshot button was
	   pressed while we weren't streaming */
	sd->snapshot_needs_reset = 1;
	sd_reset_snapshot(gspca_dev);

	sd->first_frame = 3;

	ov51x_restart(sd);
	ov51x_led_control(sd, 1);
	return gspca_dev->usb_err;
}