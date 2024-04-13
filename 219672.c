static int sd_config(struct gspca_dev *gspca_dev,
			const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam = &gspca_dev->cam;

	sd->bridge = id->driver_info & BRIDGE_MASK;
	sd->invert_led = (id->driver_info & BRIDGE_INVERT_LED) != 0;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		cam->cam_mode = ov511_vga_mode;
		cam->nmodes = ARRAY_SIZE(ov511_vga_mode);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		cam->cam_mode = ov518_vga_mode;
		cam->nmodes = ARRAY_SIZE(ov518_vga_mode);
		break;
	case BRIDGE_OV519:
		cam->cam_mode = ov519_vga_mode;
		cam->nmodes = ARRAY_SIZE(ov519_vga_mode);
		break;
	case BRIDGE_OVFX2:
		cam->cam_mode = ov519_vga_mode;
		cam->nmodes = ARRAY_SIZE(ov519_vga_mode);
		cam->bulk_size = OVFX2_BULK_SIZE;
		cam->bulk_nurbs = MAX_NURBS;
		cam->bulk = 1;
		break;
	case BRIDGE_W9968CF:
		cam->cam_mode = w9968cf_vga_mode;
		cam->nmodes = ARRAY_SIZE(w9968cf_vga_mode);
		break;
	}

	sd->frame_rate = 15;

	return 0;
}