R_API int r_core_block_size(RCore *core, int bsize) {
	return r_core_seek_size (core, core->offset, bsize);
}