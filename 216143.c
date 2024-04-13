R_API bool r_socket_spawn(RSocket *s, const char *cmd, unsigned int timeout) {
	// XXX TODO: dont use sockets, we can achieve the same with pipes
	const int port = 2000 + r_num_rand (2000);
	int childPid = r_sys_fork ();
	if (childPid == 0) {
		char *a = r_str_replace (strdup (cmd), "\\", "\\\\", true);
		int res = r_sys_cmdf ("rarun2 system=\"%s\" listen=%d", a, port);
		free (a);
#if 0
		// TODO: use the api
		char *profile = r_str_newf (
				"system=%s\n"
				"listen=%d\n", cmd, port);
		RRunProfile *rp = r_run_new (profile);
		r_run_start (rp);
		r_run_free (rp);
		free (profile);
#endif
		if (res != 0) {
			eprintf ("r_socket_spawn: rarun2 failed\n");
			exit (1);
		}
		eprintf ("r_socket_spawn: %s is dead\n", cmd);
		exit (0);
	}
	r_sys_sleep (1);
	r_sys_usleep (timeout);

	char aport[32];
	sprintf (aport, "%d", port);
	// redirect stdin/stdout/stderr
	bool sock = r_socket_connect (s, "127.0.0.1", aport, R_SOCKET_PROTO_TCP, 2000);
	if (!sock) {
		return false;
	}
#if __UNIX__
	r_sys_sleep (4);
	r_sys_usleep (timeout);

	int status = 0;
	int ret = waitpid (childPid, &status, WNOHANG);
	if (ret != 0) {
		r_socket_close (s);
		return false;
	}
#endif
	return true;
}