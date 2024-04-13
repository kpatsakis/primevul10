iasecc_chv_cache_is_verified(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_pin_status *current = NULL;
	unsigned char data_sha1[SHA_DIGEST_LENGTH];

	LOG_FUNC_CALLED(ctx);

	if (pin_cmd->pin1.data)
		SHA1(pin_cmd->pin1.data, pin_cmd->pin1.len, data_sha1);
	else
		memset(data_sha1, 0, SHA_DIGEST_LENGTH);
	sc_log_hex(ctx, "data_sha1: %s", data_sha1, SHA_DIGEST_LENGTH);

	for(current = checked_pins; current; current = current->next)
		if (current->reference == pin_cmd->pin_reference)
			break;

	if (current && !memcmp(data_sha1, current->sha1, SHA_DIGEST_LENGTH))   {
		sc_log(ctx, "PIN-%i status 'verified'", pin_cmd->pin_reference);
		return current;
	}

	sc_log(ctx, "PIN-%i status 'not verified'", pin_cmd->pin_reference);
	return NULL;
}