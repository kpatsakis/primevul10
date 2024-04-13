static int cardos_add_algs(sc_card_t *card, unsigned long flags, unsigned long ec_flags, unsigned long ext_flags)
{

	cardos_data_t * priv = (cardos_data_t *)card->drv_data;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	_sc_card_add_rsa_alg(card,  512, flags, 0);
	_sc_card_add_rsa_alg(card,  768, flags, 0);
	_sc_card_add_rsa_alg(card, 1024, flags, 0);
	if (priv->rsa_2048 == 1) {
		_sc_card_add_rsa_alg(card, 1280, flags, 0);
		_sc_card_add_rsa_alg(card, 1536, flags, 0);
		_sc_card_add_rsa_alg(card, 1792, flags, 0);
		_sc_card_add_rsa_alg(card, 2048, flags, 0);
	}

	if (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3) {
		/* Starting with CardOS 5, the card supports PIN query commands */
		card->caps |= SC_CARD_CAP_ISO7816_PIN_INFO;
		_sc_card_add_rsa_alg(card, 3072, flags, 0);
		_sc_card_add_rsa_alg(card, 4096, flags, 0);
	}

	/* TODO need to get sizes from supported_algos too */
	if (ec_flags != 0) {
		 _sc_card_add_ec_alg(card, 256, ec_flags, priv->ext_flags, NULL);
		 _sc_card_add_ec_alg(card, 384, ec_flags, priv->ext_flags, NULL);
	}

	return 0;
}