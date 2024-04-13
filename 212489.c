static GF_Err gf_isom_full_box_read(GF_Box *ptr, GF_BitStream *bs)
{
	if (ptr->registry->max_version_plus_one) {
		GF_FullBox *self = (GF_FullBox *) ptr;
		if (ptr->size<4) return GF_ISOM_INVALID_FILE;
		self->version = gf_bs_read_u8(bs);
		self->flags = gf_bs_read_u24(bs);
		ptr->size -= 4;
	}
	return GF_OK;
}