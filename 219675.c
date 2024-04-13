static void sd_stopN(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	ov51x_stop(sd);
	ov51x_led_control(sd, 0);
}