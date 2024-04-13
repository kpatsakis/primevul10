isPathExists(const char *path)
{
    struct stat sb;

    if ( (path == NULL) || (strcmp(path, "") == 0) ) return -1;

    if (lstat(path, &sb) != 0) {
	return -1;
    }

    return 0;
}