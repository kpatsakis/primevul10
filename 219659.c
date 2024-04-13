static void sethvflip(struct gspca_dev *gspca_dev, s32 hflip, s32 vflip)
{
	struct sd *sd = (struct sd *) gspca_dev;

	if (sd->gspca_dev.streaming)
		reg_w(sd, OV519_R51_RESET1, 0x0f);	/* block stream */
	i2c_w_mask(sd, OV7670_R1E_MVFP,
		OV7670_MVFP_MIRROR * hflip | OV7670_MVFP_VFLIP * vflip,
		OV7670_MVFP_MIRROR | OV7670_MVFP_VFLIP);
	if (sd->gspca_dev.streaming)
		reg_w(sd, OV519_R51_RESET1, 0x00);	/* restart stream */
}