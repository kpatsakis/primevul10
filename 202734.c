vte_sequence_handler_normal_keypad (VteTerminal *terminal, GValueArray *params)
{
	_vte_debug_print(VTE_DEBUG_KEYBOARD,
			"Leaving application keypad mode.\n");
	terminal->pvt->keypad_mode = VTE_KEYMODE_NORMAL;
}