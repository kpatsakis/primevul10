usage(void)
{
	fprintf(stderr, "usage: doas [-Lns] [-C config] [-u user]"
	    " command [args]\n");
	exit(1);
}