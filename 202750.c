vte_sequence_handler_cd (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	glong i;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	/* If the cursor is actually on the screen, clear the rest of the
	 * row the cursor is on and all of the rows below the cursor. */
	i = screen->cursor_current.row;
	if (i < _vte_ring_next(screen->row_data)) {
		/* Get the data for the row we're clipping. */
		rowdata = _vte_ring_index(screen->row_data, VteRowData *, i);
		/* Clear everything to the right of the cursor. */
		if ((rowdata != NULL) &&
		    ((glong) rowdata->cells->len > screen->cursor_current.col)) {
			g_array_set_size(rowdata->cells,
					 screen->cursor_current.col);
		}
	}
	/* Now for the rest of the lines. */
	for (i = screen->cursor_current.row + 1;
	     i < _vte_ring_next(screen->row_data);
	     i++) {
		/* Get the data for the row we're removing. */
		rowdata = _vte_ring_index(screen->row_data, VteRowData *, i);
		/* Remove it. */
		if ((rowdata != NULL) && (rowdata->cells->len > 0)) {
			g_array_set_size(rowdata->cells, 0);
		}
	}
	/* Now fill the cleared areas. */
	for (i = screen->cursor_current.row;
	     i < screen->insert_delta + terminal->row_count;
	     i++) {
		/* Retrieve the row's data, creating it if necessary. */
		if (_vte_ring_contains(screen->row_data, i)) {
			rowdata = _vte_ring_index(screen->row_data,
						  VteRowData *, i);
			g_assert(rowdata != NULL);
		} else {
			if (terminal->pvt->free_row) {
				rowdata = _vte_reset_row_data (terminal,
						terminal->pvt->free_row,
						FALSE);
			} else {
				rowdata = _vte_new_row_data(terminal);
			}
			terminal->pvt->free_row =
				_vte_ring_append(screen->row_data, rowdata);
		}
		/* Pad out the row. */
		vte_g_array_fill(rowdata->cells,
				 &screen->fill_defaults,
				 terminal->column_count);
		rowdata->soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(terminal,
				      0, terminal->column_count,
				      i, 1);
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}