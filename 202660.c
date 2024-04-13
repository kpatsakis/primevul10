vte_sequence_handler_utf_8_charset (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_set_encoding(terminal, "UTF-8");
}