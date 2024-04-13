vte_sequence_handler_ks (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->keypad_mode = VTE_KEYMODE_APPLICATION;
}