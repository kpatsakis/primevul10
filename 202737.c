vte_sequence_handler_cursor_preceding_line (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
	vte_sequence_handler_UP (terminal, params);
}