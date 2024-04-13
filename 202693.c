vte_sequence_handler_return_terminal_id (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_send_primary_device_attributes (terminal, params);
}