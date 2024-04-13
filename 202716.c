vte_terminal_emit_move_window(VteTerminal *terminal, guint x, guint y)
{
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Emitting `move-window'.\n");
	g_signal_emit_by_name(terminal, "move-window", x, y);
}