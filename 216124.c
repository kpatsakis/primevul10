R_API char *r_str_version(const char *program) {
	char *s = r_str_newf ("%s "R2_VERSION" %d @ "
			R_SYS_OS"-"
			R_SYS_ARCH"-%d git.%s\n",
			program, R2_VERSION_COMMIT,
			(R_SYS_BITS & 8)? 64: 32,
			*R2_GITTAP ? R2_GITTAP: "");
	if (*R2_GITTIP) {
		s = r_str_appendf (s, "commit: "R2_GITTIP" build: "R2_BIRTH);
	}
	return s;
}