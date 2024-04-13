int intel_virtual_engine_attach_bond(struct intel_engine_cs *engine,
				     const struct intel_engine_cs *master,
				     const struct intel_engine_cs *sibling)
{
	struct virtual_engine *ve = to_virtual_engine(engine);
	struct ve_bond *bond;
	int n;

	/* Sanity check the sibling is part of the virtual engine */
	for (n = 0; n < ve->num_siblings; n++)
		if (sibling == ve->siblings[n])
			break;
	if (n == ve->num_siblings)
		return -EINVAL;

	bond = virtual_find_bond(ve, master);
	if (bond) {
		bond->sibling_mask |= sibling->mask;
		return 0;
	}

	bond = krealloc(ve->bonds,
			sizeof(*bond) * (ve->num_bonds + 1),
			GFP_KERNEL);
	if (!bond)
		return -ENOMEM;

	bond[ve->num_bonds].master = master;
	bond[ve->num_bonds].sibling_mask = sibling->mask;

	ve->bonds = bond;
	ve->num_bonds++;

	return 0;
}