vte_sequence_handler_fs (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->status_line = FALSE;
}