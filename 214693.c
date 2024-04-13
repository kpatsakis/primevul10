}

void print_udta(GF_ISOFile *file, u32 track_number)
{
	u32 i, count;

	count =  gf_isom_get_udta_count(file, track_number);
	if (!count) return;

	fprintf(stderr, "%d UDTA types: ", count);

	for (i=0; i<count; i++) {
		u32 type;
		bin128 uuid;
		gf_isom_get_udta_type(file, track_number, i+1, &type, &uuid);
		fprintf(stderr, "%s (%d) ", gf_4cc_to_str(type), gf_isom_get_user_data_count(file, track_number, type, uuid) );
	}