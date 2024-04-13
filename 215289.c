static int iasecc_check_update_pin(struct sc_pin_cmd_data *data, struct sc_pin_cmd_pin *pin)
{
	if ((!pin->data && pin->len) || (pin->data && !pin->len))
		return SC_ERROR_INVALID_ARGUMENTS;

	if (pin->data)
		data->flags &= ~SC_PIN_CMD_USE_PINPAD;
	else
		data->flags |= SC_PIN_CMD_USE_PINPAD;

	return SC_SUCCESS;
}