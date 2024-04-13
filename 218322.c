intel_virtual_engine_get_sibling(struct intel_engine_cs *engine,
				 unsigned int sibling)
{
	struct virtual_engine *ve = to_virtual_engine(engine);

	if (sibling >= ve->num_siblings)
		return NULL;

	return ve->siblings[sibling];
}