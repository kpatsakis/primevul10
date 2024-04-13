R_API int r_core_seek_align(RCore *core, ut64 align, int times) {
	int inc = (times >= 0)? 1: -1;
	ut64 seek = core->offset;
	if (!align) {
		return false;
	}
	int diff = core->offset % align;
	if (!times) {
		diff = -diff;
	} else if (diff) {
		if (inc > 0) {
			diff += align-diff;
		} else {
			diff = -diff;
		}
		if (times) {
			times -= inc;
		}
	}
	while ((times*inc) > 0) {
		times -= inc;
		diff += (align * inc);
	}
	if (diff < 0 && -diff > seek) {
		seek = diff = 0;
	}
	return r_core_seek (core, seek + diff, 1);
}