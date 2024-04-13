static int tea6320_initialize(struct CHIPSTATE * chip)
{
	chip_write(chip, TEA6320_FFR, 0x3f);
	chip_write(chip, TEA6320_FFL, 0x3f);
	chip_write(chip, TEA6320_FRR, 0x3f);
	chip_write(chip, TEA6320_FRL, 0x3f);

	return 0;
}