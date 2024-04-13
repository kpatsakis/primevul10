static int tda8425_initialize(struct CHIPSTATE *chip)
{
	struct CHIPDESC *desc = chip->desc;
	int inputmap[4] = { /* tuner	*/ TDA8425_S1_CH2, /* radio  */ TDA8425_S1_CH1,
			    /* extern	*/ TDA8425_S1_CH1, /* intern */ TDA8425_S1_OFF};

	if (chip->c->adapter->id == I2C_HW_B_RIVA) {
		memcpy (desc->inputmap, inputmap, sizeof (inputmap));
	}
	return 0;
}