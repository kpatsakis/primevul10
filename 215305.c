iasecc_fcp_encode(struct sc_card *card, struct sc_file *file, unsigned char *out, size_t out_len)
{
	struct sc_context *ctx = card->ctx;
	unsigned char buf[0x80], type;
	unsigned char  ops[7] = {
		SC_AC_OP_DELETE, 0xFF, SC_AC_OP_ACTIVATE, SC_AC_OP_DEACTIVATE, 0xFF, SC_AC_OP_UPDATE, SC_AC_OP_READ
	};
	unsigned char smbs[8];
	size_t ii, offs = 0, amb, mask, nn_smb;

	LOG_FUNC_CALLED(ctx);

	if (file->type == SC_FILE_TYPE_DF)
		type = IASECC_FCP_TYPE_DF;
	else
		type = IASECC_FCP_TYPE_EF;

	buf[offs++] = IASECC_FCP_TAG_SIZE;
	buf[offs++] = 2;
	buf[offs++] = (file->size >> 8) & 0xFF;
	buf[offs++] = file->size & 0xFF;

	buf[offs++] = IASECC_FCP_TAG_TYPE;
	buf[offs++] = 1;
	buf[offs++] = type;

	buf[offs++] = IASECC_FCP_TAG_FID;
	buf[offs++] = 2;
	buf[offs++] = (file->id >> 8) & 0xFF;
	buf[offs++] = file->id & 0xFF;

	buf[offs++] = IASECC_FCP_TAG_SFID;
	buf[offs++] = 0;

	amb = 0, mask = 0x40, nn_smb = 0;
	for (ii = 0; ii < sizeof(ops); ii++, mask >>= 1) {
		const struct sc_acl_entry *entry;

		if (ops[ii]==0xFF)
			continue;

		entry = sc_file_get_acl_entry(file, ops[ii]);
		if (!entry)
			continue;

		sc_log(ctx, "method %X; reference %X", entry->method, entry->key_ref);
		if (entry->method == SC_AC_NEVER)
			continue;
		else if (entry->method == SC_AC_NONE)
			smbs[nn_smb++] = 0x00;
		else if (entry->method == SC_AC_CHV)
			smbs[nn_smb++] = entry->key_ref | IASECC_SCB_METHOD_USER_AUTH;
		else if (entry->method == SC_AC_SEN)
			smbs[nn_smb++] = entry->key_ref | IASECC_SCB_METHOD_USER_AUTH;
		else if (entry->method == SC_AC_SCB)
			smbs[nn_smb++] = entry->key_ref;
		else if (entry->method == SC_AC_PRO)
			smbs[nn_smb++] = entry->key_ref | IASECC_SCB_METHOD_SM;
		else
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Non supported AC method");

		amb |= mask;
		sc_log(ctx,
		       "%"SC_FORMAT_LEN_SIZE_T"u: AMB %"SC_FORMAT_LEN_SIZE_T"X; nn_smb %"SC_FORMAT_LEN_SIZE_T"u",
		       ii, amb, nn_smb);
	}

	/* TODO: Encode contactless ACLs and life cycle status for all IAS/ECC cards */
	if (card->type == SC_CARD_TYPE_IASECC_SAGEM ||
			card->type == SC_CARD_TYPE_IASECC_AMOS )  {
		unsigned char status = 0;

		buf[offs++] = IASECC_FCP_TAG_ACLS;
		buf[offs++] = 2*(2 + 1 + nn_smb);

		buf[offs++] = IASECC_FCP_TAG_ACLS_CONTACT;
		buf[offs++] = nn_smb + 1;
		buf[offs++] = amb;
		memcpy(buf + offs, smbs, nn_smb);
		offs += nn_smb;

		/* Same ACLs for contactless */
		buf[offs++] = IASECC_FCP_TAG_ACLS_CONTACTLESS;
		buf[offs++] = nn_smb + 1;
		buf[offs++] = amb;
		memcpy(buf + offs, smbs, nn_smb);
		offs += nn_smb;

		if (file->status == SC_FILE_STATUS_ACTIVATED)
			status = 0x05;
		else if (file->status == SC_FILE_STATUS_CREATION)
			status = 0x01;

		if (status)   {
			buf[offs++] = 0x8A;
			buf[offs++] = 0x01;
			buf[offs++] = status;
		}
	}
	else   {
		buf[offs++] = IASECC_FCP_TAG_ACLS;
		buf[offs++] = 2 + 1 + nn_smb;

		buf[offs++] = IASECC_FCP_TAG_ACLS_CONTACT;
		buf[offs++] = nn_smb + 1;
		buf[offs++] = amb;
		memcpy(buf + offs, smbs, nn_smb);
		offs += nn_smb;
	}

	if (out)   {
		if (out_len < offs)
			LOG_TEST_RET(ctx, SC_ERROR_BUFFER_TOO_SMALL, "Buffer too small to encode FCP");
		memcpy(out, buf, offs);
	}

	LOG_FUNC_RETURN(ctx, offs);
}