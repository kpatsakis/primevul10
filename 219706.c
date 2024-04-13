static void ovfx2_i2c_w(struct sd *sd, u8 reg, u8 value)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int ret;

	if (sd->gspca_dev.usb_err < 0)
		return;

	ret = usb_control_msg(sd->gspca_dev.dev,
			usb_sndctrlpipe(sd->gspca_dev.dev, 0),
			0x02,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			(u16) value, (u16) reg, NULL, 0, 500);

	if (ret < 0) {
		gspca_err(gspca_dev, "ovfx2_i2c_w %02x failed %d\n", reg, ret);
		sd->gspca_dev.usb_err = ret;
	}

	gspca_dbg(gspca_dev, D_USBO, "ovfx2_i2c_w %02x %02x\n", reg, value);
}