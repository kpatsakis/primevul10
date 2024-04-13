}

GF_Err dump_isom_udta(GF_ISOFile *file, char *inName, Bool is_final_name, u32 dump_udta_type, u32 dump_udta_track)
{
	u8 *data;
	FILE *t;
	bin128 uuid;
	u32 count, res;
	GF_Err e;

	memset(uuid, 0, 16);
	count = gf_isom_get_user_data_count(file, dump_udta_track, dump_udta_type, uuid);
	if (!count) {
		M4_LOG(GF_LOG_ERROR, ("No UDTA for type %s found\n", gf_4cc_to_str(dump_udta_type) ));
		return GF_NOT_FOUND;
	}

	data = NULL;
	count = 0;
	e = gf_isom_get_user_data(file, dump_udta_track, dump_udta_type, uuid, 0, &data, &count);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error dumping UDTA %s: %s\n", gf_4cc_to_str(dump_udta_type), gf_error_to_string(e) ));
		return e;
	}
	if (inName) {
		char szName[1024];
		if (is_final_name)
			strcpy(szName, inName);
		else
			sprintf(szName, "%s_%s.udta", inName, gf_4cc_to_str(dump_udta_type) );

		t = gf_fopen(szName, "wb");
		if (!t) {
			gf_free(data);
			M4_LOG(GF_LOG_ERROR, ("Cannot open file %s\n", szName ));
			return GF_IO_ERR;
		}
	} else {
		t = stdout;
	}
	res = (u32) gf_fwrite(data+8, count-8, t);
	if (inName) gf_fclose(t);
	gf_free(data);
	if (count-8 != res) {
		M4_LOG(GF_LOG_ERROR, ("Error writing udta to file\n"));
		return GF_IO_ERR;
	}