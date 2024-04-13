static void sd_stop0(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	if (!sd->gspca_dev.present)
		return;
	if (sd->bridge == BRIDGE_W9968CF)
		w9968cf_stop0(sd);

#if IS_ENABLED(CONFIG_INPUT)
	/* If the last button state is pressed, release it now! */
	if (sd->snapshot_pressed) {
		input_report_key(gspca_dev->input_dev, KEY_CAMERA, 0);
		input_sync(gspca_dev->input_dev);
		sd->snapshot_pressed = 0;
	}
#endif
	if (sd->bridge == BRIDGE_OV519)
		reg_w(sd, OV519_R57_SNAPSHOT, 0x23);
}