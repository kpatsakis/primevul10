R_API char *r_sys_whoami (char *buf) {
	char _buf[32];
	int pid = getpid ();
	int hasbuf = (buf)? 1: 0;
	if (!hasbuf) {
		buf = _buf;
	}
	sprintf (buf, "pid%d", pid);
	return hasbuf? buf: strdup (buf);
}