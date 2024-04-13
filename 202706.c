vte_sequence_handler_set_window_title (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_set_title_internal(terminal, params, FALSE, TRUE);
}