sc_oberthur_parse_tokeninfo (struct sc_pkcs15_card *p15card,
		unsigned char *buff, size_t len, int postpone_allowed)
{
	struct sc_context *ctx = p15card->card->ctx;
	char label[0x21];
	unsigned flags;
	int ii;

	LOG_FUNC_CALLED(ctx);
	if (!buff || len < 0x24)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Cannot parse token info");

	memset(label, 0, sizeof(label));

	memcpy(label, buff, 0x20);
	ii = 0x20;
	while (*(label + --ii)==' ' && ii)
		;
	*(label + ii + 1) = '\0';

	flags = *(buff + 0x22) * 0x100 + *(buff + 0x23);

	set_string(&p15card->tokeninfo->label, label);
	set_string(&p15card->tokeninfo->manufacturer_id, "Oberthur/OpenSC");

	if (flags & 0x01)
		p15card->tokeninfo->flags |= SC_PKCS15_TOKEN_PRN_GENERATION;

	sc_log(ctx, "label %s", p15card->tokeninfo->label);
	sc_log(ctx, "manufacturer_id %s", p15card->tokeninfo->manufacturer_id);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}