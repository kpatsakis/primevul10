iasecc_pin_reset(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	unsigned char scb;
	struct sc_pin_cmd_data pin_cmd;
	struct iasecc_pin_policy policy;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Reset PIN(ref:%i,lengths:%i/%i)", data->pin_reference, data->pin1.len, data->pin2.len);

	if (data->pin_type != SC_AC_CHV)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Unblock procedure can be used only with the PINs of type CHV");

	rv = iasecc_pin_get_policy(card, data, &policy);
	LOG_TEST_RET(ctx, rv, "Failed to get PIN policy");

	scb = policy.scbs[IASECC_ACLS_CHV_RESET];
	do   {
		unsigned need_all = scb & IASECC_SCB_METHOD_NEED_ALL ? 1 : 0;
		unsigned char se_num = scb & IASECC_SCB_METHOD_MASK_REF;

		if (scb & IASECC_SCB_METHOD_USER_AUTH)   {
			pin_cmd = *data;
			if (pin_cmd.puk_reference)   {
				sc_log(ctx, "Verify PIN with CHV %X", pin_cmd.puk_reference);
				pin_cmd.pin_type = SC_AC_CHV;
				pin_cmd.pin_reference = pin_cmd.puk_reference;
			} else   {
				sc_log(ctx, "Verify PIN in SE %X", se_num);
				pin_cmd.pin_type = SC_AC_SEN;
				pin_cmd.pin_reference = se_num;
			}
			rv = iasecc_pin_verify(card, &pin_cmd, tries_left);
			LOG_TEST_RET(ctx, rv, "iasecc_pin_reset() verify PUK error");

			if (!need_all)
				break;
		}

		if (scb & IASECC_SCB_METHOD_SM)   {
			rv = iasecc_sm_pin_reset(card, se_num, data);
			LOG_FUNC_RETURN(ctx, rv);
		}

		if (scb & IASECC_SCB_METHOD_EXT_AUTH)   {
			rv =  iasecc_sm_external_authentication(card, data->pin_reference, tries_left);
			LOG_TEST_RET(ctx, rv, "iasecc_pin_reset() external authentication error");
		}
	} while(0);

	/* Use iso 7816 layer for unblock, with implicit pin for PIN1 and the new PIN for PIN2 */
	pin_cmd = *data;
	pin_cmd.cmd = SC_PIN_CMD_UNBLOCK;
	pin_cmd.flags |= SC_PIN_CMD_IMPLICIT_CHANGE;
	pin_cmd.pin1.len = 0;

	rv = iasecc_pin_merge_policy(card, &pin_cmd, &pin_cmd.pin2, &policy);
	LOG_TEST_RET(ctx, rv, "Failed to update PIN2 info");

	rv = iso_ops->pin_cmd(card, &pin_cmd, tries_left);

	LOG_FUNC_RETURN(ctx, rv);
}