static void ov519_configure(struct sd *sd)
{
	static const struct ov_regvals init_519[] = {
		{ 0x5a, 0x6d }, /* EnableSystem */
		{ 0x53, 0x9b }, /* don't enable the microcontroller */
		{ OV519_R54_EN_CLK1, 0xff }, /* set bit2 to enable jpeg */
		{ 0x5d, 0x03 },
		{ 0x49, 0x01 },
		{ 0x48, 0x00 },
		/* Set LED pin to output mode. Bit 4 must be cleared or sensor
		 * detection will fail. This deserves further investigation. */
		{ OV519_GPIO_IO_CTRL0,   0xee },
		{ OV519_R51_RESET1, 0x0f },
		{ OV519_R51_RESET1, 0x00 },
		{ 0x22, 0x00 },
		/* windows reads 0x55 at this point*/
	};

	write_regvals(sd, init_519, ARRAY_SIZE(init_519));
}