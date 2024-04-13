static void ov8xx0_configure(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int rc;

	gspca_dbg(gspca_dev, D_PROBE, "starting ov8xx0 configuration\n");

	/* Detect sensor (sub)type */
	rc = i2c_r(sd, OV7610_REG_COM_I);
	if (rc < 0) {
		gspca_err(gspca_dev, "Error detecting sensor type\n");
		return;
	}
	if ((rc & 3) == 1)
		sd->sensor = SEN_OV8610;
	else
		gspca_err(gspca_dev, "Unknown image sensor version: %d\n",
			  rc & 3);
}