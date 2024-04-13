static int verify_global_checksums(struct archive_read* a) {
	return verify_checksums(a);
}