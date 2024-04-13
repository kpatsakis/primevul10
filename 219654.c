static void ov51x_handle_button(struct gspca_dev *gspca_dev, u8 state)
{
	struct sd *sd = (struct sd *) gspca_dev;

	if (sd->snapshot_pressed != state) {
#if IS_ENABLED(CONFIG_INPUT)
		input_report_key(gspca_dev->input_dev, KEY_CAMERA, state);
		input_sync(gspca_dev->input_dev);
#endif
		if (state)
			sd->snapshot_needs_reset = 1;

		sd->snapshot_pressed = state;
	} else {
		/* On the ov511 / ov519 we need to reset the button state
		   multiple times, as resetting does not work as long as the
		   button stays pressed */
		switch (sd->bridge) {
		case BRIDGE_OV511:
		case BRIDGE_OV511PLUS:
		case BRIDGE_OV519:
			if (state)
				sd->snapshot_needs_reset = 1;
			break;
		}
	}
}