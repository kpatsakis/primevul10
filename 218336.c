virtual_find_bond(struct virtual_engine *ve,
		  const struct intel_engine_cs *master)
{
	int i;

	for (i = 0; i < ve->num_bonds; i++) {
		if (ve->bonds[i].master == master)
			return &ve->bonds[i];
	}

	return NULL;
}