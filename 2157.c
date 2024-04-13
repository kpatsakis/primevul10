void get_in_jail(void)
{
	struct passwd *pw;
	uid_t uid;
	gid_t gid;

	pw = getpwnam(TMATE_JAIL_USER);
	if (!pw) {
		tmate_fatal("Cannot get the /etc/passwd entry for %s",
			    TMATE_JAIL_USER);
	}
	uid = pw->pw_uid;
	gid = pw->pw_gid;

	if (getuid() != 0)
		tmate_fatal("Need root privileges to create the jail");

	if (chroot(TMATE_WORKDIR "/jail") < 0)
		tmate_fatal("Cannot chroot()");

	if (chdir("/") < 0)
		tmate_fatal("Cannot chdir()");

#ifdef IS_LINUX
	if (unshare(CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWNET) < 0)
		tmate_fatal("Cannot create new namespace");
#endif

	if (setgroups(1, (gid_t[]){gid}) < 0)
		tmate_fatal("Cannot setgroups()");

#if defined(HAVE_SETRESGID)
	if (setresgid(gid, gid, gid) < 0)
		tmate_fatal("Cannot setresgid() %d", gid);
#elif defined(HAVE_SETREGID)
	if (setregid(gid, gid) < 0)
		tmate_fatal("Cannot setregid()");
#else
	if (setgid(gid) < 0)
		tmate_fatal("Cannot setgid()");
#endif

#if defined(HAVE_SETRESUID)
	if (setresuid(uid, uid, uid) < 0)
		tmate_fatal("Cannot setresuid()");
#elif defined(HAVE_SETREUID)
	if (setreuid(uid, uid) < 0)
		tmate_fatal("Cannot setreuid()");
#else
	if (setuid(uid) < 0)
		tmate_fatal("Cannot setuid()");
#endif

	nice(1);

	tmate_debug("Dropped priviledges to %s (%d,%d), jailed in %s",
		    TMATE_JAIL_USER, uid, gid, TMATE_WORKDIR "/jail");
}