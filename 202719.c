vte_sequence_handler_character_position_absolute (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_ch);
}