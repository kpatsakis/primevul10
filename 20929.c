unlock_mtab (void) {
	if (we_created_lockfile) {
		close(lockfile_fd);
		lockfile_fd = -1;
		unlink (_PATH_MOUNTED_LOCK);
		we_created_lockfile = 0;
	}
}