vte_sequence_handler_mr (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.reverse = 1;
}