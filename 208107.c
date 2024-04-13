free_pamconf(pamtab_t *cp)
{
	int i;

	if (cp) {
		if (cp->pam_service)
			free(cp->pam_service);
		if (cp->module_path)
			free(cp->module_path);
		for (i = 0; i < cp->module_argc; i++) {
			if (cp->module_argv[i])
				free(cp->module_argv[i]);
		}
		if (cp->module_argc > 0)
			free(cp->module_argv);
		if (cp->function_ptr)
			free(cp->function_ptr);

		free(cp);
	}
}