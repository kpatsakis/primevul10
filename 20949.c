my_endmntent(FILE *stream, off_t size)
{
	int rc, fd;

	fd = fileno(stream);
	if (fd < 0)
		return -EBADF;

	rc = fflush(stream);
	if (!rc)
		rc = fsync(fd);

	/* truncate file back to "size" -- best effort here */
	if (rc) {
		rc = errno;
		ftruncate(fd, size);
	}

	endmntent(stream);
	return rc;
}