R_API const char *r_str_rwx_i(int rwx) {
	if (rwx < 0 || rwx >= R_ARRAY_SIZE (rwxstr)) {
		rwx = 0;
	}
	return rwxstr[rwx % 24]; // 15 for srwx
}