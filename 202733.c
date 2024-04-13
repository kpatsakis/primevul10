vte_sequence_handler_send_primary_device_attributes (VteTerminal *terminal, GValueArray *params)
{
	/* Claim to be a VT220 with only national character set support. */
	vte_terminal_feed_child(terminal, "\e[?62;9;c", -1);
}