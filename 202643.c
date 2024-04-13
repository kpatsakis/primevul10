vte_sequence_handler_set_icon_title (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_set_title_internal(terminal, params, TRUE, FALSE);
}