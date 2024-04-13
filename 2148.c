void close_fds_except(int *fd_to_preserve, int num_fds)
{
	int fd, i, preserve;

	for (fd = 0; fd < 1024; fd++) {
		preserve = 0;
		for (i = 0; i < num_fds; i++)
			if (fd_to_preserve[i] == fd)
				preserve = 1;

		if (!preserve)
			close(fd);
	}
}