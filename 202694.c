vte_sequence_handler_ae (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->alternate_charset = FALSE;
}