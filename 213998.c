static void nhmldump_pck_property(GF_NHMLDumpCtx *ctx, u32 p4cc, const char *pname, const GF_PropertyValue *att)
{
	u32 i;
	char nhml[1024];
	char pval[GF_PROP_DUMP_ARG_SIZE];
	if (!pname) pname = gf_props_4cc_get_name(p4cc);

	sprintf(nhml, "%s=\"", pname ? pname : gf_4cc_to_str(p4cc));
	gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

	switch (att->type) {
	case GF_PROP_DATA:
	case GF_PROP_CONST_DATA:
	case GF_PROP_DATA_NO_COPY:
		sprintf(nhml, "0x");
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		for (i=0; i<att->value.data.size; i++) {
			sprintf(nhml, "%02X", (unsigned char) att->value.data.ptr[i]);
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		nhml[0] = 0;
		break;
	default:
		sprintf(nhml, "%s", gf_props_dump_val(att, pval, GF_FALSE, NULL) );
		break;
	}
	gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
	sprintf(nhml, "\"");
	gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
}