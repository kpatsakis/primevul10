R_API bool r_sys_mkdir(const char *dir) {
	bool ret;

	if (r_sandbox_enable (0)) {
		return false;
	}
#if __WINDOWS__
	LPTSTR dir_ = r_sys_conv_utf8_to_win (dir);

	ret = CreateDirectory (dir_, NULL) != 0;
	free (dir_);
#else
	ret = mkdir (dir, 0755) != -1;
#endif
	return ret;
}