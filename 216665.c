static int cardos_check_sw(sc_card_t *card, unsigned int sw1, unsigned int sw2)
{
	const int err_count = sizeof(cardos_errors)/sizeof(cardos_errors[0]);
	int i;
			        
	for (i = 0; i < err_count; i++) {
		if (cardos_errors[i].SWs == ((sw1 << 8) | sw2)) {
			if ( cardos_errors[i].errorstr ) 
				sc_log(card->ctx,  "%s\n",
				 	cardos_errors[i].errorstr);
			return cardos_errors[i].errorno;
		}
	}

        sc_log(card->ctx,  "Unknown SWs; SW1=%02X, SW2=%02X\n", sw1, sw2);
	return SC_ERROR_CARD_CMD_FAILED;
}