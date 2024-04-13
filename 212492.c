GF_Err gf_isom_box_write_header(GF_Box *ptr, GF_BitStream *bs)
{
	u64 start;
	if (! bs || !ptr) return GF_BAD_PARAM;
	if (!ptr->size) return GF_ISOM_INVALID_FILE;

	start = gf_bs_get_position(bs);
	if (ptr->size > 0xFFFFFFFF) {
		gf_bs_write_u32(bs, 1);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->size);
	}
	gf_bs_write_u32(bs, ptr->type);
	if (ptr->type == GF_ISOM_BOX_TYPE_UUID) {
		u32 i;
		char uuid[16];
		char strUUID[32];

		switch (((GF_UUIDBox*)ptr)->internal_4cc) {
		case GF_ISOM_BOX_UUID_TENC:
			memcpy(strUUID, "8974dbce7be74c5184f97148f9882554", 32);
			break;
		case GF_ISOM_BOX_UUID_PSEC:
			memcpy(strUUID, "A2394F525A9B4F14A2446C427C648DF4", 32);
			break;
		case GF_ISOM_BOX_UUID_MSSM:
			memcpy(strUUID, "A5D40B30E81411DDBA2F0800200C9A66", 32);
			break;
		case GF_ISOM_BOX_UUID_PSSH:
			memcpy(strUUID, "D08A4F1810F34A82B6C832D8ABA183D3", 32);
			break;
		case GF_ISOM_BOX_UUID_TFXD:
			memcpy(strUUID, "6D1D9B0542D544E680E2141DAFF757B2", 32);
			break;
		default:
			memset(strUUID, 0, 32);
			break;
		}

		for (i = 0; i < 16; i++) {
			char t[3];
			t[2] = 0;
			t[0] = strUUID[2*i];
			t[1] = strUUID[2*i+1];
			uuid[i] = (u8) strtol(t, NULL, 16);
		}

		gf_bs_write_data(bs, uuid, 16);
	}
	if (ptr->size > 0xFFFFFFFF)
		gf_bs_write_u64(bs, ptr->size);

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Written Box type %s size "LLD" start "LLD"\n", gf_4cc_to_str(ptr->type), LLD_CAST ptr->size, LLD_CAST start));

	return GF_OK;
}