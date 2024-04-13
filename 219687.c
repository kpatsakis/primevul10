static void setautogain(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;

	i2c_w_mask(sd, 0x13, val ? 0x05 : 0x00, 0x05);
}