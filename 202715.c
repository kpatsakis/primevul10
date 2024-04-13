vte_sequence_handler_cursor_next_line (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
	vte_sequence_handler_DO (terminal, params);
}