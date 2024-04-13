vte_sequence_handler_ue (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.underline = 0;
}