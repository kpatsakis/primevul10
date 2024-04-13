vte_remove_line_internal(VteTerminal *terminal, glong position)
{
	if (_vte_ring_next(terminal->pvt->screen->row_data) > position) {
		if (terminal->pvt->free_row)
			_vte_free_row_data (terminal->pvt->free_row);

		terminal->pvt->free_row = _vte_ring_remove(
				terminal->pvt->screen->row_data,
				position,
				FALSE);
	}
}