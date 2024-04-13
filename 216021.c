R_API void r_sys_info_free(RSysInfo *si) {
	free (si->sysname);
	free (si->nodename);
	free (si->release);
	free (si->version);
	free (si->machine);
	free (si);
}