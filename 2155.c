static int check_owned_directory_mode(const char *path, mode_t expected_mode)
{
	struct stat stat;
	if (lstat(path, &stat))
		return -1;

	if (!S_ISDIR(stat.st_mode))
		return -1;

	if (stat.st_uid != getuid())
		return -1;

	if ((stat.st_mode & 07777) != expected_mode)
		return -1;

	return 0;
}