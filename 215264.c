static void dmar_invalid_context_cache_global(struct dmar_drhd_rt *dmar_unit)
{
	dmar_invalid_context_cache(dmar_unit, 0U, 0U, 0U, DMAR_CIRG_GLOBAL);
}