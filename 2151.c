void setup_ncurse(int fd, const char *name)
{
	int error;
	if (setupterm((char *)name, fd, &error) != OK)
		tmate_fatal("Cannot setup terminal");
}