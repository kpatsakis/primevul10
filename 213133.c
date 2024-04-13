static int mywrite(const ut8 *buf, int len) {
	return r_cons_memcat ((const char *)buf, len);
}