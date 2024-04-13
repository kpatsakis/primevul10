vte_sequence_handler_full_reset (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_reset(terminal, TRUE, TRUE);
}