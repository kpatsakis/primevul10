

static void dump_key_info(const u8 *key_info, u32 key_info_size, Bool is_protected)
{
	if (!key_info) return;
	u32 j, k, kpos=3;
	u32 nb_keys = 1;
	if (key_info[0]) {
		nb_keys = key_info[1];
		nb_keys <<= 8;
		nb_keys |= key_info[2];
	}
	for (k=0; k<nb_keys; k++) {
		u8 constant_iv_size=0;
		u8 iv_size=key_info[kpos+1];
		fprintf(stderr, "\t\tKID");
		if (nb_keys>1)
			fprintf(stderr, "%d", k+1);
		fprintf(stderr, " ");
		for (j=0; j<16; j++) fprintf(stderr, "%02X", key_info[kpos+1+j]);
		kpos+=17;
		if (!iv_size && is_protected) {
			constant_iv_size = key_info[1];
			kpos += 1 + constant_iv_size;
		}
		fprintf(stderr, " - %sIV size %d \n", constant_iv_size ? "const " : "", constant_iv_size ? constant_iv_size : iv_size);