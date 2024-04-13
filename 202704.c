vte_sequence_handler_cl (VteTerminal *terminal, GValueArray *params)
{
	_vte_terminal_clear_screen (terminal);
	_vte_terminal_home_cursor (terminal);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}