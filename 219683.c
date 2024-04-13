static void setfreq(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;

	setfreq_i(sd, val);

	/* Ugly but necessary */
	if (sd->bridge == BRIDGE_W9968CF)
		w9968cf_set_crop_window(sd);
}