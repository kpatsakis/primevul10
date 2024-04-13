vte_sequence_handler_im (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->insert_mode = TRUE;
}