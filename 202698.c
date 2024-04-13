vte_sequence_handler_ke (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->keypad_mode = VTE_KEYMODE_NORMAL;
}