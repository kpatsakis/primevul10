iasecc_pin_change(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_pin_cmd_data pin_cmd;
	struct iasecc_pin_policy policy;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Change PIN(ref:%i,type:0x%X,lengths:%i/%i)",
	       data->pin_reference, data->pin_type, data->pin1.len, data->pin2.len);

	if (data->pin_type != SC_AC_CHV)   {
		sc_log(ctx, "Can not change non-CHV PINs");
		LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);
	}

	/*
	 * Verify the original PIN. This would normally not be needed since it is implicitly done
	 * by the card when executing a PIN change command. But we must go through our verification
	 * function in order to handle secure messaging setup, if enabled for the PIN. The
	 * verification is skipped for PIN-pads (which do not work with SM anyway), to avoid the
	 * user having to enter the PIN twice.
	 */
	pin_cmd = *data;
	pin_cmd.cmd = SC_PIN_CMD_VERIFY;

	rv = iasecc_pin_merge_policy(card, &pin_cmd, &pin_cmd.pin1, &policy);
	LOG_TEST_RET(ctx, rv, "Failed to update PIN1 info");

	if (!(pin_cmd.flags & SC_PIN_CMD_USE_PINPAD)) {
		rv = iasecc_chv_verify(card, &pin_cmd, policy.scbs, tries_left);
		LOG_TEST_RET(ctx, rv, "PIN CHV verification error");
	}

	/*
	 * To keep things simple, assume that we can use the same PIN parameters for the new PIN as
	 * for the old one, ignoring the ones specified by the caller, with the exception of the
	 * PIN prompt and the PIN data itself. Note that the old PIN is re-verified since the
	 * IAS-ECC specification has no implicit verification for the PIN change command. This also
	 * forces us to always use PIN-pad for the second PIN if the first one was input on a
	 * PIN-pad.
	 */
	pin_cmd.cmd = SC_PIN_CMD_CHANGE;
	pin_cmd.pin2 = pin_cmd.pin1;
	pin_cmd.pin2.prompt = data->pin2.prompt;
	if (pin_cmd.flags & SC_PIN_CMD_USE_PINPAD) {
		pin_cmd.pin2.data = NULL;
		pin_cmd.pin2.len = 0;
	} else {
		pin_cmd.pin2.data = data->pin2.data;
		pin_cmd.pin2.len = data->pin2.len;
	}

	rv = iasecc_check_update_pin(&pin_cmd, &pin_cmd.pin2);
	LOG_TEST_RET(ctx, rv, "Invalid PIN2");

	rv = iso_ops->pin_cmd(card, &pin_cmd, tries_left);
	LOG_FUNC_RETURN(ctx, rv);
}