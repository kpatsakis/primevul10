static int chip_write_masked(struct CHIPSTATE *chip,
			     int subaddr, int val, int mask)
{
	if (mask != 0) {
		if (subaddr < 0) {
			val = (chip->shadow.bytes[1] & ~mask) | (val & mask);
		} else {
			if (subaddr + 1 >= ARRAY_SIZE(chip->shadow.bytes)) {
				v4l_info(chip->c,
					"Tried to access a non-existent register: %d\n",
					subaddr);
				return -EINVAL;
			}

			val = (chip->shadow.bytes[subaddr+1] & ~mask) | (val & mask);
		}
	}
	return chip_write(chip, subaddr, val);
}