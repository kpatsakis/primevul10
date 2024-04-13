static void reg_w_mask(struct sd *sd,
			u16 index,
			u8 value,
			u8 mask)
{
	int ret;
	u8 oldval;

	if (mask != 0xff) {
		value &= mask;			/* Enforce mask on value */
		ret = reg_r(sd, index);
		if (ret < 0)
			return;

		oldval = ret & ~mask;		/* Clear the masked bits */
		value |= oldval;		/* Set the desired bits */
	}
	reg_w(sd, index, value);
}