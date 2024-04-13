vte_sequence_handler_ei (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->insert_mode = FALSE;
}