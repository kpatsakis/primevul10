static struct virtual_engine *to_virtual_engine(struct intel_engine_cs *engine)
{
	GEM_BUG_ON(!intel_engine_is_virtual(engine));
	return container_of(engine, struct virtual_engine, base);
}