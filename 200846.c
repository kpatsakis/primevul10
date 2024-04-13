static int ta8874z_checkit(struct CHIPSTATE *chip)
{
	int rc;
	rc = chip_read(chip);
	return ((rc & 0x1f) == 0x1f) ? 1 : 0;
}