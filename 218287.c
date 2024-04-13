static const u8 *reg_offsets(const struct intel_engine_cs *engine)
{
	/*
	 * The gen12+ lists only have the registers we program in the basic
	 * default state. We rely on the context image using relative
	 * addressing to automatic fixup the register state between the
	 * physical engines for virtual engine.
	 */
	GEM_BUG_ON(INTEL_GEN(engine->i915) >= 12 &&
		   !intel_engine_has_relative_mmio(engine));

	if (engine->class == RENDER_CLASS) {
		if (INTEL_GEN(engine->i915) >= 12)
			return gen12_rcs_offsets;
		else if (INTEL_GEN(engine->i915) >= 11)
			return gen11_rcs_offsets;
		else
			return gen8_rcs_offsets;
	} else {
		if (INTEL_GEN(engine->i915) >= 12)
			return gen12_xcs_offsets;
		else if (INTEL_GEN(engine->i915) >= 9)
			return gen9_xcs_offsets;
		else
			return gen8_xcs_offsets;
	}
}