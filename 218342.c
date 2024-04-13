intel_execlists_clone_virtual(struct i915_gem_context *ctx,
			      struct intel_engine_cs *src)
{
	struct virtual_engine *se = to_virtual_engine(src);
	struct intel_context *dst;

	dst = intel_execlists_create_virtual(ctx,
					     se->siblings,
					     se->num_siblings);
	if (IS_ERR(dst))
		return dst;

	if (se->num_bonds) {
		struct virtual_engine *de = to_virtual_engine(dst->engine);

		de->bonds = kmemdup(se->bonds,
				    sizeof(*se->bonds) * se->num_bonds,
				    GFP_KERNEL);
		if (!de->bonds) {
			intel_context_put(dst);
			return ERR_PTR(-ENOMEM);
		}

		de->num_bonds = se->num_bonds;
	}

	return dst;
}