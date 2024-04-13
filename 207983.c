static void *unsupported_operation(const char *opname) THROWSPEC
{
	fprintf(stderr, "nedmalloc: The operation %s is not supported under this build configuration\n", opname);
	abort();
	return 0;
}