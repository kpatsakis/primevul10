static void fixup_transceive_length(const struct sc_card *card,
		struct sc_apdu *apdu)
{
	if (card == NULL || apdu == NULL) {
		return;
	}

	if (apdu->lc > sc_get_max_send_size(card)) {
		/* The lower layers will automatically do chaining */
		apdu->flags |= SC_APDU_FLAGS_CHAINING;
	}

	if (apdu->le > sc_get_max_recv_size(card)) {
		/* The lower layers will automatically do a GET RESPONSE, if possible.
		 * All other workarounds must be carried out by the upper layers. */
		apdu->le = sc_get_max_recv_size(card);
	}
}