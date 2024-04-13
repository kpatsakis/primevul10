vte_sequence_handler_UP (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_up);
}