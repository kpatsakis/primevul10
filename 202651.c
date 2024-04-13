vte_sequence_handler_md (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.bold = 1;
	terminal->pvt->screen->defaults.attr.half = 0;
}