static void ov51x_led_control(struct sd *sd, int on)
{
	if (sd->invert_led)
		on = !on;

	switch (sd->bridge) {
	/* OV511 has no LED control */
	case BRIDGE_OV511PLUS:
		reg_w(sd, R511_SYS_LED_CTL, on);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		reg_w_mask(sd, R518_GPIO_OUT, 0x02 * on, 0x02);
		break;
	case BRIDGE_OV519:
		reg_w_mask(sd, OV519_GPIO_DATA_OUT0, on, 1);
		break;
	}
}