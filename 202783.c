vte_sequence_handler_horizontal_and_vertical_position (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cm);
}