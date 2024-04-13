static void setautobright(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;

	i2c_w_mask(sd, 0x2d, val ? 0x10 : 0x00, 0x10);
}