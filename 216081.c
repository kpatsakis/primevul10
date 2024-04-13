R_API const char *r_str_sysbits(const int v) {
	switch (v) {
	case R_SYS_BITS_8: return "8";
	case R_SYS_BITS_16: return "16";
	case R_SYS_BITS_32: return "32";
	case R_SYS_BITS_64: return "64";
	case R_SYS_BITS_16 | R_SYS_BITS_32: return "16,32";
	case R_SYS_BITS_16 | R_SYS_BITS_32 | R_SYS_BITS_64: return "16,32,64";
	case R_SYS_BITS_32 | R_SYS_BITS_64: return "32,64";
	}
	return "?";
}