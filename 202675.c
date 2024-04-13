vte_sequence_handler_ve (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->cursor_visible = TRUE;
}