vte_sequence_handler_return_terminal_status (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_feed_child(terminal, "", 0);
}