vte_sequence_handler_cursor_position (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cm);
}