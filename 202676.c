vte_sequence_handler_soft_reset (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_reset(terminal, FALSE, FALSE);
}