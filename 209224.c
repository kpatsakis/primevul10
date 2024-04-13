WriteWarningRaw(const char *msg, int fd, size_t wantWrite, ssize_t written)
{
    fprintf(stderr, "%s: Warning: wanted to write %" PRIuz
            " bytes, wrote %ld to file descriptor %d\n",
            msg, wantWrite, (long)written, fd);
}