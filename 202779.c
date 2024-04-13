vte_sequence_handler_SR (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_sr);
}