GF_Err gf_isom_box_dump_ex(void *ptr, FILE * trace, u32 box_4cc)
{
	GF_Box *a = (GF_Box *) ptr;

	if (skip_box_dump_del) return GF_OK;

	if (!a) {
		if (box_4cc) {
			fprintf(trace, "<!--ERROR: NULL Box Found, expecting %s -->\n", gf_4cc_to_str(box_4cc) );
		} else {
			fprintf(trace, "<!--ERROR: NULL Box Found-->\n");
		}
		return GF_OK;
	}
	if (!a->registry) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isom] trying to dump box %s not registered\n", gf_4cc_to_str(a->type) ));
		return GF_ISOM_INVALID_FILE;
	}
	a->registry->dump_fn(a, trace);
	return GF_OK;
}