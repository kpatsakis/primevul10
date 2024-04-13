virtio_find_cr(const struct config_reg *p_cr_array, u_int array_size,
	       int offset) {
	u_int hi, lo, mid;
	const struct config_reg *cr;

	lo = 0;
	hi = array_size - 1;
	while (hi >= lo) {
		mid = (hi + lo) >> 1;
		cr = p_cr_array + mid;
		if (cr->offset == offset)
			return cr;
		if (cr->offset < offset)
			lo = mid + 1;
		else
			hi = mid - 1;
	}
	return NULL;
}