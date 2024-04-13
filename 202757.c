vte_insert_line_internal(VteTerminal *terminal, glong position)
{
	VteRowData *row, *old_row;
	old_row = terminal->pvt->free_row;
	/* Pad out the line data to the insertion point. */
	while (_vte_ring_next(terminal->pvt->screen->row_data) < position) {
		if (old_row) {
			row = _vte_reset_row_data (terminal, old_row, TRUE);
		} else {
			row = _vte_new_row_data_sized(terminal, TRUE);
		}
		old_row = _vte_ring_append(terminal->pvt->screen->row_data, row);
	}
	/* If we haven't inserted a line yet, insert a new one. */
	if (old_row) {
		row = _vte_reset_row_data (terminal, old_row, TRUE);
	} else {
		row = _vte_new_row_data_sized(terminal, TRUE);
	}
	if (_vte_ring_next(terminal->pvt->screen->row_data) >= position) {
		old_row = _vte_ring_insert(terminal->pvt->screen->row_data,
				 position, row);
	} else {
		old_row =_vte_ring_append(terminal->pvt->screen->row_data, row);
	}
	terminal->pvt->free_row = old_row;
}