iasecc_chv_cache_verified(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_pin_status *pin_status = NULL, *current = NULL;

	LOG_FUNC_CALLED(ctx);

	for(current = checked_pins; current; current = current->next)
		if (current->reference == pin_cmd->pin_reference)
			break;

	if (current)   {
		sc_log(ctx, "iasecc_chv_cache_verified() current PIN-%i", current->reference);
		pin_status = current;
	}
	else   {
		pin_status = calloc(1, sizeof(struct iasecc_pin_status));
		if (!pin_status)
			LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot callocate PIN status info");
		sc_log(ctx, "iasecc_chv_cache_verified() allocated %p", pin_status);
	}

	pin_status->reference = pin_cmd->pin_reference;
	if (pin_cmd->pin1.data)
		SHA1(pin_cmd->pin1.data, pin_cmd->pin1.len, pin_status->sha1);
	else
		memset(pin_status->sha1, 0, SHA_DIGEST_LENGTH);

	sc_log_hex(ctx, "iasecc_chv_cache_verified() sha1(PIN)", pin_status->sha1, SHA_DIGEST_LENGTH);

	if (!current)   {
		if (!checked_pins)   {
			checked_pins = pin_status;
		}
		else   {
		checked_pins->prev = pin_status;
			pin_status->next = checked_pins;
			checked_pins = pin_status;
		}
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}