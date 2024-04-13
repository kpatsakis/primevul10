ssize_t Freadall(FD_t fd, void * buf, ssize_t size)
{
    ssize_t total = 0;
    ssize_t nb = 0;
    char * bufp = buf;

    while (total < size) {
	nb = Fread(bufp, 1, size - total, fd);

	if (nb == 0 || (nb < 0 && !RETRY_ERROR(errno))) {
	    total = nb;
	    break;
	}

	if (nb > 0) {
	    bufp += nb;
	    total += nb;
	}
    }

    return total;
}