GF_Err gf_isom_box_dump_start(GF_Box *a, const char *name, FILE * trace)
{
	fprintf(trace, "<%s ", name);
	if (a->size > 0xFFFFFFFF) {
		fprintf(trace, "LargeSize=\""LLU"\" ", LLU_CAST a->size);
	} else {
		fprintf(trace, "Size=\"%u\" ", (u32) a->size);
	}
	if (a->type==GF_ISOM_BOX_TYPE_UNKNOWN) {
		fprintf(trace, "Type=\"%s\" ", gf_4cc_to_str(((GF_UnknownBox*)a)->original_4cc));
	} else {
		fprintf(trace, "Type=\"%s\" ", gf_4cc_to_str(a->type));
	}

	if (a->type == GF_ISOM_BOX_TYPE_UUID) {
		u32 i;
		fprintf(trace, "UUID=\"{");
		for (i=0; i<16; i++) {
			fprintf(trace, "%02X", (unsigned char) ((GF_UUIDBox*)a)->uuid[i]);
			if ((i<15) && (i%4)==3) fprintf(trace, "-");
		}
		fprintf(trace, "}\" ");
	}

	if (a->registry->max_version_plus_one) {
		fprintf(trace, "Version=\"%d\" Flags=\"%d\" ", ((GF_FullBox*)a)->version,((GF_FullBox*)a)->flags);
	}

	fprintf(trace, "Specification=\"%s\" ", a->registry->spec);
	fprintf(trace, "Container=\"%s\" ", a->registry->parents_4cc);
	//disable all box dumping until end of this box
	if (use_dump_mode) {
		skip_box_dump_del = GF_TRUE;
	}
	return GF_OK;
}