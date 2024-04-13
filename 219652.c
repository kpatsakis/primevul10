static void i2c_w_mask(struct sd *sd,
			u8 reg,
			u8 value,
			u8 mask)
{
	int rc;
	u8 oldval;

	value &= mask;			/* Enforce mask on value */
	rc = i2c_r(sd, reg);
	if (rc < 0)
		return;
	oldval = rc & ~mask;		/* Clear the masked bits */
	value |= oldval;		/* Set the desired bits */
	i2c_w(sd, reg, value);
}