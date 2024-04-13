const char *get_command_path(const char *cmd)
{
	const char *p;
	int i;
	for (i = 0; valid_cmds[i] != NULL; i++) {
		if (access(valid_cmds[i], F_OK) == -1)
			continue;
		p = strrchr(valid_cmds[i], '/') + 1;
		if (strcmp(p, cmd) == 0)
			return valid_cmds[i];
	}
	return NULL;
}