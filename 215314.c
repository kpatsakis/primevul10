iasecc_pin_verify(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	unsigned type = data->pin_type;
	unsigned reference = data->pin_reference;
	struct sc_pin_cmd_data pin_cmd;
	struct iasecc_pin_policy policy;
	int tries_before_verify = -1;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "Verify PIN(type:%X,ref:%i,data(len:%i,%p)",
	       type, reference, data->pin1.len, data->pin1.data);

	if (type == SC_AC_AUT)   {
		rv =  iasecc_sm_external_authentication(card, reference, tries_left);
		LOG_FUNC_RETURN(ctx, rv);
	}

	if (type == SC_AC_SCB)   {
		if (reference & IASECC_SCB_METHOD_USER_AUTH)   {
			type = SC_AC_SEN;
			reference = reference & IASECC_SCB_METHOD_MASK_REF;
		}
	}

	if (type == SC_AC_SEN)   {
		type = SC_AC_CHV;
		rv = iasecc_se_at_to_chv_reference(card, reference,  &reference);
		LOG_TEST_RET(ctx, rv, "SE AT to CHV reference error");
	}

	if (type != SC_AC_CHV)   {
		sc_log(ctx, "Do not try to verify non CHV PINs");
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);
	}

	pin_cmd = *data;
	pin_cmd.pin_type = SC_AC_CHV;
	pin_cmd.pin_reference = reference;
	pin_cmd.cmd = SC_PIN_CMD_VERIFY;

	rv = iasecc_pin_get_status(card, &pin_cmd, tries_left);
	if (data->pin1.data && !data->pin1.len)
		LOG_FUNC_RETURN(ctx, rv);

	if (!rv)   {
		if (pin_cmd.pin1.logged_in == SC_PIN_STATE_LOGGED_IN)
			if (iasecc_chv_cache_is_verified(card, &pin_cmd))
				LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	}
	else if (rv != SC_ERROR_SECURITY_STATUS_NOT_SATISFIED)   {
		LOG_FUNC_RETURN(ctx, rv);
	}

	iasecc_chv_cache_clean(card, &pin_cmd);

	rv = iasecc_pin_merge_policy(card, &pin_cmd, &pin_cmd.pin1, &policy);
	LOG_TEST_RET(ctx, rv, "Failed to update PIN1 info");

	/* PIN-pads work best with fixed-size lengths. Use PIN padding when length is available. */
	if (pin_cmd.flags & SC_PIN_CMD_USE_PINPAD) {
		tries_before_verify = pin_cmd.pin1.tries_left;
		if (policy.stored_length > 0)
			iasecc_set_pin_padding(&pin_cmd, &pin_cmd.pin1, policy.stored_length);
	}

	rv = iasecc_chv_verify(card, &pin_cmd, policy.scbs, tries_left);

	/*
	 * Detect and log PIN-pads which don't handle variable-length PIN - special case where they
	 * forward the CHV verify command with Lc = 0 to the card, without updating Lc. An IAS-ECC
	 * card responds to this command by returning the number of attempts left, without
	 * decreasing the counter.
	 */
	if ((pin_cmd.flags & SC_PIN_CMD_USE_PINPAD) && !(pin_cmd.flags & SC_PIN_CMD_NEED_PADDING)) {
		if (rv == SC_ERROR_PIN_CODE_INCORRECT && pin_cmd.pin1.tries_left == tries_before_verify) {
			SC_TEST_RET(ctx, SC_LOG_DEBUG_VERBOSE, rv,
				    "PIN-pad reader does not support variable-length PIN");
		}
	}

	LOG_TEST_RET(ctx, rv, "PIN CHV verification error");

	rv = iasecc_chv_cache_verified(card, &pin_cmd);

	LOG_FUNC_RETURN(ctx, rv);
}