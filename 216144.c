static bool checkExtract() {
#if __WINDOWS__
	if (r_sys_cmd ("expand -? >nul") != 0) {
		return false;
	}
#else
	if (r_sys_cmd ("cabextract -v > /dev/null") != 0) {
		return false;
	}
#endif
	return true;
}