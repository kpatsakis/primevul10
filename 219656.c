static int sd_init_controls(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *)gspca_dev;
	struct v4l2_ctrl_handler *hdl = &gspca_dev->ctrl_handler;

	gspca_dev->vdev.ctrl_handler = hdl;
	v4l2_ctrl_handler_init(hdl, 10);
	if (valid_controls[sd->sensor].has_brightness)
		sd->brightness = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_BRIGHTNESS, 0,
			sd->sensor == SEN_OV7660 ? 6 : 255, 1,
			sd->sensor == SEN_OV7660 ? 3 : 127);
	if (valid_controls[sd->sensor].has_contrast) {
		if (sd->sensor == SEN_OV7660)
			v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
				V4L2_CID_CONTRAST, 0, 6, 1, 3);
		else
			v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
				V4L2_CID_CONTRAST, 0, 255, 1,
				(sd->sensor == SEN_OV6630 ||
				 sd->sensor == SEN_OV66308AF) ? 200 : 127);
	}
	if (valid_controls[sd->sensor].has_sat)
		v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_SATURATION, 0,
			sd->sensor == SEN_OV7660 ? 4 : 255, 1,
			sd->sensor == SEN_OV7660 ? 2 : 127);
	if (valid_controls[sd->sensor].has_exposure)
		gspca_dev->exposure = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_EXPOSURE, 0, 255, 1, 127);
	if (valid_controls[sd->sensor].has_hvflip) {
		sd->hflip = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_HFLIP, 0, 1, 1, 0);
		sd->vflip = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_VFLIP, 0, 1, 1, 0);
	}
	if (valid_controls[sd->sensor].has_autobright)
		sd->autobright = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_AUTOBRIGHTNESS, 0, 1, 1, 1);
	if (valid_controls[sd->sensor].has_autogain)
		gspca_dev->autogain = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_AUTOGAIN, 0, 1, 1, 1);
	if (valid_controls[sd->sensor].has_freq) {
		if (sd->sensor == SEN_OV7670)
			sd->freq = v4l2_ctrl_new_std_menu(hdl, &sd_ctrl_ops,
				V4L2_CID_POWER_LINE_FREQUENCY,
				V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
				V4L2_CID_POWER_LINE_FREQUENCY_AUTO);
		else
			sd->freq = v4l2_ctrl_new_std_menu(hdl, &sd_ctrl_ops,
				V4L2_CID_POWER_LINE_FREQUENCY,
				V4L2_CID_POWER_LINE_FREQUENCY_60HZ, 0, 0);
	}
	if (sd->bridge == BRIDGE_W9968CF)
		sd->jpegqual = v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_JPEG_COMPRESSION_QUALITY,
			QUALITY_MIN, QUALITY_MAX, 1, QUALITY_DEF);

	if (hdl->error) {
		gspca_err(gspca_dev, "Could not initialize controls\n");
		return hdl->error;
	}
	if (gspca_dev->autogain)
		v4l2_ctrl_auto_cluster(3, &gspca_dev->autogain, 0, true);
	if (sd->autobright)
		v4l2_ctrl_auto_cluster(2, &sd->autobright, 0, false);
	if (sd->hflip)
		v4l2_ctrl_cluster(2, &sd->hflip);
	return 0;
}