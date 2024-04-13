vte_sequence_handler_request_terminal_parameters (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_feed_child(terminal, "\e[?x", -1);
}