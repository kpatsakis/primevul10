static void core_post_write_callback(void *user, ut64 maddr, ut8 *bytes, int cnt) {
	RCore *core = (RCore *)user;
	RBinSection *sec;
	ut64 vaddr;

	if (!r_config_get_i (core->config, "asm.cmt.patch")) {
		return;
	}

	char *hex_pairs = r_hex_bin2strdup (bytes, cnt);
	if (!hex_pairs) {
		eprintf ("core_post_write_callback: Cannot obtain hex pairs\n");
		return;
	}

	char *comment = r_str_newf ("patch: %d byte(s) (%s)", cnt, hex_pairs);
	free (hex_pairs);
	if (!comment) {
		eprintf ("core_post_write_callback: Cannot create comment\n");
		return;
	}

	if ((sec = r_bin_get_section_at (r_bin_cur_object (core->bin), maddr, false))) {
		vaddr = maddr + sec->vaddr - sec->paddr;
	} else {
		vaddr = maddr;
	}

	r_meta_add (core->anal, R_META_TYPE_COMMENT, vaddr, vaddr, comment);
	free (comment);
}