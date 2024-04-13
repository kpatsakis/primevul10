GF_Err gf_isom_box_write_listing(GF_Box *a, GF_BitStream *bs)
{
	if (!a) return GF_BAD_PARAM;
	if (!a->registry) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Write invalid box type %s without registry\n", gf_4cc_to_str(a->type) ));
		return GF_ISOM_INVALID_FILE;
	}
	return a->registry->write_fn(a, bs);
}