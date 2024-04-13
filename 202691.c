vte_sequence_handler_mh (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.half = 1;
	terminal->pvt->screen->defaults.attr.bold = 0;
}