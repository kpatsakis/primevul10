lock_mtab (void) {
	int i;
	struct timespec waittime;
	struct timeval maxtime;
	char linktargetfile[MOUNTLOCK_LINKTARGET_LTH];

	if (!signals_have_been_setup) {
		int sig = 0;
		struct sigaction sa;

		sa.sa_handler = handler;
		sa.sa_flags = 0;
		sigfillset (&sa.sa_mask);

		while (sigismember (&sa.sa_mask, ++sig) != -1
		       && sig != SIGCHLD) {
			if (sig == SIGALRM)
				sa.sa_handler = setlkw_timeout;
			else
				sa.sa_handler = handler;
			sigaction (sig, &sa, (struct sigaction *) 0);
		}
		signals_have_been_setup = 1;
	}

	sprintf(linktargetfile, MOUNTLOCK_LINKTARGET, getpid ());

	i = open (linktargetfile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	if (i < 0) {
		/* linktargetfile does not exist (as a file)
		   and we cannot create it. Read-only filesystem?
		   Too many files open in the system?
		   Filesystem full? */
		return EX_FILEIO;
	}
	close(i);

	maxtime = mono_time();
	maxtime.tv_sec += MOUNTLOCK_MAXTIME;

	waittime.tv_sec = 0;
	waittime.tv_nsec = (1000 * MOUNTLOCK_WAITTIME);

	/* Repeat until it was us who made the link */
	while (!we_created_lockfile) {
		struct timeval now;
		struct flock flock;
		int errsv, j;

		j = link(linktargetfile, _PATH_MOUNTED_LOCK);
		errsv = errno;

		if (j == 0)
			we_created_lockfile = 1;

		if (j < 0 && errsv != EEXIST) {
			(void) unlink(linktargetfile);
			return EX_FILEIO;
		}

		lockfile_fd = open (_PATH_MOUNTED_LOCK, O_WRONLY);

		if (lockfile_fd < 0) {
			/* Strange... Maybe the file was just deleted? */
			now = mono_time();
			if (errno == ENOENT && now.tv_sec < maxtime.tv_sec) {
				we_created_lockfile = 0;
				continue;
			}
			(void) unlink(linktargetfile);
			return EX_FILEIO;
		}

		flock.l_type = F_WRLCK;
		flock.l_whence = SEEK_SET;
		flock.l_start = 0;
		flock.l_len = 0;

		if (j == 0) {
			/* We made the link. Now claim the lock. If we can't
			 * get it, continue anyway
			 */
			fcntl (lockfile_fd, F_SETLK, &flock);
			(void) unlink(linktargetfile);
		} else {
			/* Someone else made the link. Wait. */
			now = mono_time();
			if (now.tv_sec < maxtime.tv_sec) {
				alarm(maxtime.tv_sec - now.tv_sec);
				if (fcntl (lockfile_fd, F_SETLKW, &flock) == -1) {
					(void) unlink(linktargetfile);
					return EX_FILEIO;
				}
				alarm(0);
				nanosleep(&waittime, NULL);
			} else {
				(void) unlink(linktargetfile);
				return EX_FILEIO;
			}
			close(lockfile_fd);
		}
	}
	return 0;
}