vte_sequence_handler_as (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->alternate_charset = TRUE;
}