static inline int shmem_parse_mpol(char *value, int *policy, nodemask_t *policy_nodes)
{
	char *nodelist = strchr(value, ':');
	int err = 1;

	if (nodelist) {
		/* NUL-terminate policy string */
		*nodelist++ = '\0';
		if (nodelist_parse(nodelist, *policy_nodes))
			goto out;
		if (!nodes_subset(*policy_nodes, node_states[N_HIGH_MEMORY]))
			goto out;
	}
	if (!strcmp(value, "default")) {
		*policy = MPOL_DEFAULT;
		/* Don't allow a nodelist */
		if (!nodelist)
			err = 0;
	} else if (!strcmp(value, "prefer")) {
		*policy = MPOL_PREFERRED;
		/* Insist on a nodelist of one node only */
		if (nodelist) {
			char *rest = nodelist;
			while (isdigit(*rest))
				rest++;
			if (!*rest)
				err = 0;
		}
	} else if (!strcmp(value, "bind")) {
		*policy = MPOL_BIND;
		/* Insist on a nodelist */
		if (nodelist)
			err = 0;
	} else if (!strcmp(value, "interleave")) {
		*policy = MPOL_INTERLEAVE;
		/*
		 * Default to online nodes with memory if no nodelist
		 */
		if (!nodelist)
			*policy_nodes = node_states[N_HIGH_MEMORY];
		err = 0;
	}
out:
	/* Restore string for error message */
	if (nodelist)
		*--nodelist = ':';
	return err;
}