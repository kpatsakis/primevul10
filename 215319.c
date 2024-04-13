iasecc_pin_get_info(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_pin_policy policy;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_pin_get_info(card:%p)", card);

	/*
	 * Get PIN status first and thereafter update with info from PIN policy, when available.
	 * The first one is typically used for the PIN verification status and number of remaining
	 * tries, and the second one for the maximum tries. If a field is present in both, the
	 * policy takes precedence.
	 */
	rv = iasecc_pin_get_status(card, data, tries_left);
	LOG_TEST_RET(ctx, rv, "Failed to get PIN status");

	rv = iasecc_pin_get_policy(card, data, &policy);
	LOG_TEST_RET(ctx, rv, "Failed to get PIN policy");

	/*
	 * We only care about the tries_xxx fields in the PIN policy, since the other ones are not
	 * commonly expected or used in a SC_PIN_CMD_GET_INFO response.	Note that max_tries is
	 * always taken from the policy, since it is never expected to be available in status (it
	 * is set to -1 when not available in policy).
	 */
	data->pin1.max_tries = policy.tries_maximum;
	if (policy.tries_remaining >= 0)
		data->pin1.tries_left = policy.tries_remaining;

	if (tries_left)
		*tries_left = data->pin1.tries_left;

	LOG_FUNC_RETURN(ctx, rv);
}