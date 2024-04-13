R_API int r_core_search_cb(RCore *core, ut64 from, ut64 to, RCoreSearchCallback cb) {
	int ret, len = core->blocksize;
	ut8 *buf;
	if ((buf = malloc (len))) {
		while (from < to) {
			ut64 delta = to-from;
			if (delta < len) {
				len = (int)delta;
			}
			if (!r_io_read_at (core->io, from, buf, len)) {
				eprintf ("Cannot read at 0x%"PFMT64x"\n", from);
				break;
			}
			for (ret = 0; ret < len;) {
				int done = cb (core, from, buf+ret, len-ret);
				if (done < 1) { /* interrupted */
					free (buf);
					return false;
				}
				ret += done;
			}
			from += len;
		}
		free (buf);
	} else {
		eprintf ("Cannot allocate blocksize\n");
	}
	return true;
}