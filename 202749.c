_vte_terminal_clear_above_current (VteTerminal *terminal)
{
	VteRowData *rowdata;
	long i;
	VteScreen *screen;
	screen = terminal->pvt->screen;
	/* If the cursor is actually on the screen, clear data in the row
	 * which corresponds to the cursor. */
	for (i = screen->insert_delta; i < screen->cursor_current.row; i++) {
		if (_vte_ring_next(screen->row_data) > i) {
			guint len;
			/* Get the data for the row we're erasing. */
			rowdata = _vte_ring_index(screen->row_data,
						  VteRowData *, i);
			g_assert(rowdata != NULL);
			/* Remove it. */
			len = rowdata->cells->len;
			if (len > 0) {
				g_array_set_size(rowdata->cells, 0);
			}
			/* Add new cells until we fill the row. */
			vte_g_array_fill(rowdata->cells,
					 &screen->fill_defaults,
					 terminal->column_count);
			rowdata->soft_wrapped = 0;
			/* Repaint the row. */
			_vte_invalidate_cells(terminal,
					0, terminal->column_count, i, 1);
		}
	}
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}