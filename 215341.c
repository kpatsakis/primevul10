static void iasecc_set_pin_padding(struct sc_pin_cmd_data *data, struct sc_pin_cmd_pin *pin,
				   size_t pad_len)
{
	if (data->flags & SC_PIN_CMD_NEED_PADDING)
		return;

	pin->pad_length = pad_len;
	pin->pad_char = 0xff;
	data->flags |= SC_PIN_CMD_NEED_PADDING;
}