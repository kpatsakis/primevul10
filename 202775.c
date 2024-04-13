vte_sequence_handler_us (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.underline = 1;
}