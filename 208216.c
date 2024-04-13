xd3_mainerror(int err_num) {
#ifndef _WIN32
	const char* x = xd3_strerror (err_num);
	if (x != NULL)
	  {
	    return x;
	  }
	return strerror(err_num);
#else
	static char err_buf[256];
	const char* x = xd3_strerror (err_num);
	if (x != NULL)
	  {
	    return x;
	  }
	memset (err_buf, 0, 256);
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM |
		       FORMAT_MESSAGE_IGNORE_INSERTS,
		       NULL, err_num,
		       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		       err_buf, 256, NULL);
	if (err_buf[0] != 0 && err_buf[strlen(err_buf) - 1] == '\n')
	  {
	    err_buf[strlen(err_buf) - 1] = 0;
	  }
	return err_buf;
#endif
}