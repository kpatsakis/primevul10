static int reg_r8(struct sd *sd,
		  u16 index)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int ret;

	if (sd->gspca_dev.usb_err < 0)
		return -1;

	/* Avoid things going to fast for the bridge with a xhci host */
	udelay(150);
	ret = usb_control_msg(sd->gspca_dev.dev,
			usb_rcvctrlpipe(sd->gspca_dev.dev, 0),
			1,			/* REQ_IO */
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0, index, sd->gspca_dev.usb_buf, 8, 500);

	if (ret >= 0) {
		ret = sd->gspca_dev.usb_buf[0];
	} else {
		gspca_err(gspca_dev, "reg_r8 %02x failed %d\n", index, ret);
		sd->gspca_dev.usb_err = ret;
		/*
		 * Make sure the buffer is zeroed to avoid uninitialized
		 * values.
		 */
		memset(gspca_dev->usb_buf, 0, 8);
	}

	return ret;
}