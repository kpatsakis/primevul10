static int sd_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct gspca_dev *gspca_dev =
		container_of(ctrl->handler, struct gspca_dev, ctrl_handler);
	struct sd *sd = (struct sd *)gspca_dev;

	gspca_dev->usb_err = 0;

	if (!gspca_dev->streaming)
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		setbrightness(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		setcontrast(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_POWER_LINE_FREQUENCY:
		setfreq(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_AUTOBRIGHTNESS:
		if (ctrl->is_new)
			setautobright(gspca_dev, ctrl->val);
		if (!ctrl->val && sd->brightness->is_new)
			setbrightness(gspca_dev, sd->brightness->val);
		break;
	case V4L2_CID_SATURATION:
		setcolors(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_HFLIP:
		sethvflip(gspca_dev, ctrl->val, sd->vflip->val);
		break;
	case V4L2_CID_AUTOGAIN:
		if (ctrl->is_new)
			setautogain(gspca_dev, ctrl->val);
		if (!ctrl->val && gspca_dev->exposure->is_new)
			setexposure(gspca_dev, gspca_dev->exposure->val);
		break;
	case V4L2_CID_JPEG_COMPRESSION_QUALITY:
		return -EBUSY; /* Should never happen, as we grab the ctrl */
	}
	return gspca_dev->usb_err;
}