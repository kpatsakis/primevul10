_vte_terminal_scroll_text (VteTerminal *terminal, int scroll_amount)
{
	VteRowData *row, *old_row;
	long start, end, i;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->insert_delta + screen->scrolling_region.start;
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		start = screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	old_row = terminal->pvt->free_row;
	while (_vte_ring_next(screen->row_data) <= end) {
		if (old_row) {
			row = _vte_reset_row_data (terminal, old_row, FALSE);
		} else {
			row = _vte_new_row_data_sized(terminal, FALSE);
		}
		old_row = _vte_ring_append(terminal->pvt->screen->row_data, row);
	}
	terminal->pvt->free_row = old_row;
	if (scroll_amount > 0) {
		for (i = 0; i < scroll_amount; i++) {
			vte_remove_line_internal(terminal, end);
			vte_insert_line_internal(terminal, start);
		}
	} else {
		for (i = 0; i < -scroll_amount; i++) {
			vte_remove_line_internal(terminal, start);
			vte_insert_line_internal(terminal, end);
		}
	}

	/* Update the display. */
	_vte_terminal_scroll_region(terminal, start, end - start + 1,
				   scroll_amount);

	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_inserted_flag = TRUE;
	terminal->pvt->text_deleted_flag = TRUE;
}