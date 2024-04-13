mtab_unusable(void)
{
	struct stat mstat;

	if(lstat(_PATH_MOUNTED, &mstat))
		return errno;
	else if (S_ISLNK(mstat.st_mode))
		return EMLINK;
	return 0;
}