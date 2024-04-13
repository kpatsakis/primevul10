static void virtual_engine_initial_hint(struct virtual_engine *ve)
{
	int swp;

	/*
	 * Pick a random sibling on starting to help spread the load around.
	 *
	 * New contexts are typically created with exactly the same order
	 * of siblings, and often started in batches. Due to the way we iterate
	 * the array of sibling when submitting requests, sibling[0] is
	 * prioritised for dequeuing. If we make sure that sibling[0] is fairly
	 * randomised across the system, we also help spread the load by the
	 * first engine we inspect being different each time.
	 *
	 * NB This does not force us to execute on this engine, it will just
	 * typically be the first we inspect for submission.
	 */
	swp = prandom_u32_max(ve->num_siblings);
	if (!swp)
		return;

	swap(ve->siblings[swp], ve->siblings[0]);
	if (!intel_engine_has_relative_mmio(ve->siblings[0]))
		virtual_update_register_offsets(ve->context.lrc_reg_state,
						ve->siblings[0]);
}