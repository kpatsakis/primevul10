vte_sequence_handler_DC (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_dc);
}