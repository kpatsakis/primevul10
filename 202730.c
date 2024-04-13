vte_sequence_handler_cb (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	long i;
	VteScreen *screen;
	struct vte_charcell *pcell;
	screen = terminal->pvt->screen;

	/* Get the data for the row which the cursor points to. */
	rowdata = _vte_terminal_ensure_row(terminal);
	/* Clear the data up to the current column with the default
	 * attributes.  If there is no such character cell, we need
	 * to add one. */
	for (i = 0; i <= screen->cursor_current.col; i++) {
		if (i < (glong) rowdata->cells->len) {
			/* Muck with the cell in this location. */
			pcell = &g_array_index(rowdata->cells,
					       struct vte_charcell,
					       i);
			*pcell = screen->color_defaults;
		} else {
			/* Add new cells until we have one here. */
			g_array_append_val(rowdata->cells,
					   screen->color_defaults);
		}
	}
	/* Repaint this row. */
	_vte_invalidate_cells(terminal,
			      0, screen->cursor_current.col+1,
			      screen->cursor_current.row, 1);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}