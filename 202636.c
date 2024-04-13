vte_sequence_handler_vs (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->cursor_visible = TRUE; /* FIXME: should be *more*
						 visible. */
}