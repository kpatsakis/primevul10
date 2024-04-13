vte_sequence_handler_SF (VteTerminal *terminal, GValueArray *params)
{
	/* XXX implement this directly in _vte_terminal_cursor_down */
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_sf);
}