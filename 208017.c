isPathSecure(const char *path)
{
    struct stat sb;

    /* no path => unreal, that's possible ! */
    if (path == NULL) return -1;

    /* file doesn't exist or access denied = secure */
    /* fopen will fail */
    if (lstat(path, &sb) != 0) return 0;

    /* File is not a regular file => unsecure */
    if ( S_ISLNK(sb.st_mode ) ) return -1;
    if ( S_ISDIR(sb.st_mode ) ) return -1;
    if ( S_ISCHR(sb.st_mode ) ) return -1;
    if ( S_ISBLK(sb.st_mode ) ) return -1;
    if ( S_ISFIFO(sb.st_mode ) ) return -1;
    if ( S_ISSOCK(sb.st_mode ) ) return -1;

    return 0;
}