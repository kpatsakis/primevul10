vte_sequence_handler_ta (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long newcol, col;

	/* Calculate which column is the next tab stop. */
	screen = terminal->pvt->screen;
	newcol = col = screen->cursor_current.col;

	g_assert (col >= 0);

	if (terminal->pvt->tabstops != NULL) {
		/* Find the next tabstop. */
		for (newcol++; newcol < VTE_TAB_MAX; newcol++) {
			if (_vte_terminal_get_tabstop(terminal, newcol)) {
				break;
			}
		}
	}

	/* If we have no tab stops or went past the end of the line, stop
	 * at the right-most column. */
	if (newcol >= terminal->column_count) {
		newcol = terminal->column_count - 1;
	}

	/* but make sure we don't move cursor back (bug #340631) */
	if (col < newcol) {
		VteRowData *rowdata = _vte_terminal_ensure_row (terminal);

		/* Smart tab handling: bug 353610
		 *
		 * If we currently don't have any cells in the space this
		 * tab creates, we try to make the tab character copyable,
		 * by appending a single tab char with lots of fragment
		 * cells following it.
		 *
		 * Otherwise, just append empty cells that will show up
		 * as a space each.
		 */

		/* Get rid of trailing empty cells: bug 545924 */
		if ((glong) rowdata->cells->len > col)
		{
			struct vte_charcell *cell;
			guint i;
			for (i = rowdata->cells->len; (glong) i > col; i--) {
				cell = &g_array_index(rowdata->cells,
						      struct vte_charcell, i - 1);
				if (cell->attr.fragment || cell->c != 0)
					break;
			}
			g_array_set_size(rowdata->cells, i);
		}

		if ((glong) rowdata->cells->len <= col)
		  {
		    struct vte_charcell cell;

		    vte_g_array_fill (rowdata->cells,
				      &screen->fill_defaults,
				      col);

		    cell.attr = screen->fill_defaults.attr;
		    cell.attr.invisible = 1; /* FIXME: bug 499944 */
		    cell.attr.columns = newcol - col;
		    if (cell.attr.columns != newcol - col)
		      {
		        /* number of columns doesn't fit one cell. skip
			 * fancy tabs
			 */
		       goto fallback_tab;
		      }
		    cell.c = '\t';
		    g_array_append_vals(rowdata->cells, &cell, 1);

		    cell.attr = screen->fill_defaults.attr;
		    cell.attr.fragment = 1;
		    vte_g_array_fill (rowdata->cells,
				      &cell,
				newcol);
		  }
		else
		  {
		  fallback_tab:
		    vte_g_array_fill (rowdata->cells,
				      &screen->fill_defaults,
				      newcol);
		  }

		_vte_invalidate_cells (terminal,
				screen->cursor_current.col,
				newcol - screen->cursor_current.col,
				screen->cursor_current.row, 1);
		screen->cursor_current.col = newcol;
	}
}