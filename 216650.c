cardos_pin_cmd(struct sc_card *card, struct sc_pin_cmd_data *data,
		 int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	int rv;

	LOG_FUNC_CALLED(card->ctx);

	data->flags |= SC_PIN_CMD_NEED_PADDING;
	data->pin_reference |= 0x80;

	sc_log(ctx, "PIN_CMD(cmd:%i, ref:%i)", data->cmd, data->pin_reference);
	sc_log(ctx,
	       "PIN1(max:%"SC_FORMAT_LEN_SIZE_T"u, min:%"SC_FORMAT_LEN_SIZE_T"u)",
	       data->pin1.max_length, data->pin1.min_length);
	sc_log(ctx,
	       "PIN2(max:%"SC_FORMAT_LEN_SIZE_T"u, min:%"SC_FORMAT_LEN_SIZE_T"u)",
	       data->pin2.max_length, data->pin2.min_length);

	/* FIXME: the following values depend on what pin length was
	 * used when creating the BS objects */
	if (data->pin1.max_length == 0)
		data->pin1.max_length = 8;
	if (data->pin2.max_length == 0)
		data->pin2.max_length = 8;

	rv = iso_ops->pin_cmd(card, data, tries_left);
	LOG_FUNC_RETURN(ctx, rv);
}