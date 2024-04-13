vte_sequence_handler_bl (VteTerminal *terminal, GValueArray *params)
{
	_vte_terminal_beep (terminal);
	g_signal_emit_by_name(terminal, "beep");
}