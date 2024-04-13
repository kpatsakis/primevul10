R_API int r_str_write(int fd, const char *b) {
	return write (fd, b, strlen (b));
}