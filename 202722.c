vte_sequence_handler_mb (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.blink = 1;
}