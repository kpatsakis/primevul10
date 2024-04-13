vte_sequence_handler_vi (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->cursor_visible = FALSE;
}