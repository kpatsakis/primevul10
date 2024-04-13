vte_sequence_handler_mk (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.invisible = 1;
}