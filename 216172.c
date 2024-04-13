static bool r_sys_mkdir(const char *path) {
	LPTSTR path_ = r_sys_conv_utf8_to_utf16 (path);
	bool ret = CreateDirectory (path_, NULL);

	free (path_);
	return ret;
}