vte_sequence_handler_dec_device_status_report (VteTerminal *terminal, GValueArray *params)
{
	GValue *value;
	VteScreen *screen;
	long param;
	char buf[128];

	screen = terminal->pvt->screen;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			param = g_value_get_long(value);
			switch (param) {
			case 6:
				/* Send the cursor position. */
				g_snprintf(buf, sizeof(buf),
					   _VTE_CAP_CSI "?%ld;%ldR",
					   screen->cursor_current.row + 1 -
					   screen->insert_delta,
					   screen->cursor_current.col + 1);
				vte_terminal_feed_child(terminal, buf, -1);
				break;
			case 15:
				/* Send printer status -- 10 = ready,
				 * 11 = not ready.  We don't print. */
				vte_terminal_feed_child(terminal, _VTE_CAP_CSI "?11n", -1);
				break;
			case 25:
				/* Send UDK status -- 20 = locked,
				 * 21 = not locked.  I don't even know what
				 * that means, but punt anyway. */
				vte_terminal_feed_child(terminal, _VTE_CAP_CSI "?20n", -1);
				break;
			case 26:
				/* Send keyboard status.  50 = no locator. */
				vte_terminal_feed_child(terminal, _VTE_CAP_CSI "?50n", -1);
				break;
			default:
				break;
			}
		}
	}
}