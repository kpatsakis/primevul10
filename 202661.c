vte_sequence_handler_so (VteTerminal *terminal, GValueArray *params)
{
	char *standout;

	/* Standout may be mapped to another attribute, so attempt to do
	 * the Right Thing here.
	 *
	 * If the standout sequence is the same as another sequence, do what
	 * we'd do for that other sequence instead. */

	standout = _vte_termcap_find_string(terminal->pvt->termcap,
					    terminal->pvt->emulation,
					    "so");
	g_assert(standout != NULL);

	if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mb") /* blink */)
		vte_sequence_handler_mb (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "md") /* bold */)
		vte_sequence_handler_md (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mh") /* half */)
		vte_sequence_handler_mh (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mr") /* reverse */)
		vte_sequence_handler_mr (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "us") /* underline */)
		vte_sequence_handler_us (terminal, params);
	else {
		/* Otherwise just set standout mode. */
		terminal->pvt->screen->defaults.attr.standout = 1;
	}

	g_free(standout);
}