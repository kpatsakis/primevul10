vte_sequence_handler_kb (VteTerminal *terminal, GValueArray *params)
{
	/* Move the cursor left. */
	vte_sequence_handler_le (terminal, params);
}