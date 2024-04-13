static int tda9873_checkit(struct CHIPSTATE *chip)
{
	int rc;

	if (-1 == (rc = chip_read2(chip,254)))
		return 0;
	return (rc & ~0x1f) == 0x80;
}