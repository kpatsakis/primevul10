}

void print_udta(GF_ISOFile *file, u32 track_number, Bool has_itags)
{
	u32 i, count;

	count =  gf_isom_get_udta_count(file, track_number);
	if (!count) return;

	if (has_itags) {
		for (i=0; i<count; i++) {
			u32 type;
			bin128 uuid;
			gf_isom_get_udta_type(file, track_number, i+1, &type, &uuid);
			if (type == GF_ISOM_BOX_TYPE_META) {
				count--;
				break;
			}
		}
		if (!count) return;
	}

	fprintf(stderr, "%d UDTA types: ", count);

	for (i=0; i<count; i++) {
		u32 j, type, nb_items, first=GF_TRUE;
		bin128 uuid;
		gf_isom_get_udta_type(file, track_number, i+1, &type, &uuid);
		nb_items = gf_isom_get_user_data_count(file, track_number, type, uuid);
		fprintf(stderr, "%s (%d) ", gf_4cc_to_str(type), nb_items);
		for (j=0; j<nb_items; j++) {
			u8 *udta=NULL;
			u32 udta_size;
			gf_isom_get_user_data(file, track_number, type, uuid, j+1, &udta, &udta_size);
			if (!udta) continue;
			if (udta_size && gf_utf8_is_legal(udta, udta_size)) {
				u32 idx;
				if (first) {
					fprintf(stderr, "\n");
					first = GF_FALSE;
				}
				fprintf(stderr, "\t");
				for (idx=0; idx<udta_size; idx++) {
					if (!udta[idx]) break;
					fprintf(stderr, "%c", udta[idx]);
				}
				fprintf(stderr, "\n");
			}
			gf_free(udta);
		}
	}