static int sd_isoc_init(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	switch (sd->bridge) {
	case BRIDGE_OVFX2:
		if (gspca_dev->pixfmt.width != 800)
			gspca_dev->cam.bulk_size = OVFX2_BULK_SIZE;
		else
			gspca_dev->cam.bulk_size = 7 * 4096;
		break;
	}
	return 0;
}