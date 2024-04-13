static ut64 numget(RCore *core, const char *k) {
	return r_num_math (core->num, k);
}