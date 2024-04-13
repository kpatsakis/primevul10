static void DeBackSlashify(char *cp) {
    char *dp = cp;
    while (*cp) {
        if (*cp == '\\') {
            *dp = '/';
        } else
            *dp = *cp;
        cp++; dp++;
    }
    *dp = 0;
}