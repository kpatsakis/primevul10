iasecc_pin_get_status(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_pin_cmd_data info;
	int rv;

	LOG_FUNC_CALLED(ctx);

	if (data->pin_type != SC_AC_CHV)
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "PIN type is not supported for status");

	memset(&info, 0, sizeof(info));
	info.cmd = SC_PIN_CMD_GET_INFO;
	info.pin_type = data->pin_type;
	info.pin_reference = data->pin_reference;

	rv = iso_ops->pin_cmd(card, &info, tries_left);
	LOG_TEST_RET(ctx, rv, "Failed to get PIN info");

	data->pin1.max_tries = info.pin1.max_tries;
	data->pin1.tries_left = info.pin1.tries_left;
	data->pin1.logged_in = info.pin1.logged_in;

	LOG_FUNC_RETURN(ctx, rv);
}