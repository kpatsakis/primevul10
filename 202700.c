vte_sequence_handler_screen_alignment_test (VteTerminal *terminal, GValueArray *params)
{
	long row;
	VteRowData *rowdata, *old_row;
	VteScreen *screen;
	struct vte_charcell cell;

	screen = terminal->pvt->screen;

	for (row = terminal->pvt->screen->insert_delta;
	     row < terminal->pvt->screen->insert_delta + terminal->row_count;
	     row++) {
		/* Find this row. */
		old_row = terminal->pvt->free_row;
		while (_vte_ring_next(screen->row_data) <= row) {
			if (old_row) {
				rowdata = _vte_reset_row_data (terminal, old_row, FALSE);
			} else {
				rowdata = _vte_new_row_data(terminal);
			}
			old_row = _vte_ring_append(screen->row_data, rowdata);
		}
		terminal->pvt->free_row = old_row;
		_vte_terminal_adjust_adjustments(terminal);
		rowdata = _vte_ring_index(screen->row_data, VteRowData *, row);
		g_assert(rowdata != NULL);
		/* Clear this row. */
		if (rowdata->cells->len > 0) {
			g_array_set_size(rowdata->cells, 0);
		}
		_vte_terminal_emit_text_deleted(terminal);
		/* Fill this row. */
		cell.c = 'E';
		cell.attr = screen->basic_defaults.attr;
		cell.attr.columns = 1;
		vte_g_array_fill(rowdata->cells, &cell, terminal->column_count);
		_vte_terminal_emit_text_inserted(terminal);
	}
	_vte_invalidate_all(terminal);

	/* We modified the display, so make a note of it for completeness. */
	terminal->pvt->text_modified_flag = TRUE;
}