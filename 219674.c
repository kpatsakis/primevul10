static int sd_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct gspca_dev *gspca_dev =
		container_of(ctrl->handler, struct gspca_dev, ctrl_handler);
	struct sd *sd = (struct sd *)gspca_dev;

	gspca_dev->usb_err = 0;

	switch (ctrl->id) {
	case V4L2_CID_AUTOGAIN:
		gspca_dev->exposure->val = i2c_r(sd, 0x10);
		break;
	}
	return 0;
}