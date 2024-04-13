iasecc_pin_merge_policy(struct sc_card *card, struct sc_pin_cmd_data *data,
			struct sc_pin_cmd_pin *pin, struct iasecc_pin_policy *policy)
{
	struct sc_context *ctx = card->ctx;
	size_t pad_len = 0;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_pin_merge_policy(card:%p)", card);

	rv = iasecc_check_update_pin(data, pin);
	LOG_TEST_RET(ctx, rv, "Invalid PIN");

	rv = iasecc_pin_get_policy(card, data, policy);
	LOG_TEST_RET(ctx, rv, "Failed to get PIN policy");

	/* Some cards obviously use the min/max length fields to signal PIN padding */
	if (policy->min_length > 0 && policy->min_length == policy->max_length) {
		pad_len = policy->min_length;
		policy->min_length = 0;
	}

	/* Take the most limited values of min/max lengths */
	if (policy->min_length > 0 && (size_t) policy->min_length > pin->min_length)
		pin->min_length = policy->min_length;
	if (policy->max_length > 0 && (!pin->max_length || (size_t) policy->max_length < pin->max_length))
		pin->max_length = policy->max_length;

	/* Set PIN padding if needed and not already set by the caller */
	if (pad_len)
		iasecc_set_pin_padding(data, pin, pad_len);

	LOG_FUNC_RETURN(ctx, rv);
}