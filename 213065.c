R_API int r_core_seek_size(RCore *core, ut64 addr, int bsize) {
	ut8 *bump;
	int ret = false;
	if (bsize < 0) {
		return false;
	}
	if (bsize == core->blocksize) {
		return true;
	}
	if (r_sandbox_enable (0)) {
		// TODO : restrict to filesize?
		if (bsize > 1024*32) {
			eprintf ("Sandbox mode restricts blocksize bigger than 32k\n");
			return false;
		}
	}
	if (bsize > core->blocksize_max) {
		eprintf ("Block size %d is too big\n", bsize);
		return false;
	}
	core->offset = addr;
	if (bsize < 1) {
		bsize = 1;
	} else if (core->blocksize_max && bsize>core->blocksize_max) {
		eprintf ("bsize is bigger than `bm`. dimmed to 0x%x > 0x%x\n",
			bsize, core->blocksize_max);
		bsize = core->blocksize_max;
	}
	bump = realloc (core->block, bsize + 1);
	if (!bump) {
		eprintf ("Oops. cannot allocate that much (%u)\n", bsize);
		ret = false;
	} else {
		ret = true;
		core->block = bump;
		core->blocksize = bsize;
		memset (core->block, 0xff, core->blocksize);
		r_core_block_read (core);
	}
	return ret;
}