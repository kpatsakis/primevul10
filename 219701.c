static int sd_set_jcomp(struct gspca_dev *gspca_dev,
			const struct v4l2_jpegcompression *jcomp)
{
	struct sd *sd = (struct sd *) gspca_dev;

	if (sd->bridge != BRIDGE_W9968CF)
		return -ENOTTY;

	v4l2_ctrl_s_ctrl(sd->jpegqual, jcomp->quality);
	return 0;
}