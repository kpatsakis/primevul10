static int cardos_init(sc_card_t *card)
{
	cardos_data_t * priv = NULL;
	unsigned long flags = 0;
	size_t data_field_length;
	sc_apdu_t apdu;
	u8 rbuf[2];
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	priv = calloc(1, sizeof(cardos_data_t));
	if (!priv)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
	card->drv_data = priv;

	card->name = "Atos CardOS";
	card->cla = 0x00;

	/* let user override flags and type from opensc.conf */
	/* user can override card->type too.*/
	if (card->flags) {
	    flags = card->flags;
	} else {

		/* Set up algorithm info. */
		flags = 0;
		if (card->type == SC_CARD_TYPE_CARDOS_V5_0) {
			flags |= SC_ALGORITHM_RSA_PAD_PKCS1;
		} else if(card->type == SC_CARD_TYPE_CARDOS_V5_3) {
			flags |= SC_ALGORITHM_RSA_RAW
				| SC_ALGORITHM_RSA_HASH_NONE
				| SC_ALGORITHM_ONBOARD_KEY_GEN;
		} else {
			flags |= SC_ALGORITHM_RSA_RAW
				| SC_ALGORITHM_RSA_HASH_NONE
				| SC_ALGORITHM_NEED_USAGE
				| SC_ALGORITHM_ONBOARD_KEY_GEN;
		}
	}

	priv->flags = flags;

	if (card->type == SC_CARD_TYPE_CARDOS_M4_2) {
		r = cardos_have_2048bit_package(card);
		if (r < 0) {
			r = SC_ERROR_INVALID_CARD;
			goto err;
		}
		if (r == 1)
			priv->rsa_2048 = 1;
		card->caps |= SC_CARD_CAP_APDU_EXT;
	} else if (card->type == SC_CARD_TYPE_CARDOS_M4_3
		|| card->type == SC_CARD_TYPE_CARDOS_M4_2B
		|| card->type == SC_CARD_TYPE_CARDOS_M4_2C
		|| card->type == SC_CARD_TYPE_CARDOS_M4_4
		|| card->type == SC_CARD_TYPE_CARDOS_V5_0
		|| card->type == SC_CARD_TYPE_CARDOS_V5_3) {
		priv->rsa_2048 = 1;
		card->caps |= SC_CARD_CAP_APDU_EXT;
		/* TODO check this. EC only if in supported_algo */
		priv->ext_flags = SC_ALGORITHM_EXT_EC_NAMEDCURVE | SC_ALGORITHM_EXT_EC_UNCOMPRESES;
	}

	/* probe DATA FIELD LENGTH with GET DATA */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0x01, 0x8D);
	apdu.le = sizeof rbuf;
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);

	r = sc_transmit_apdu(card, &apdu);
	if (r < 0)
		LOG_TEST_GOTO_ERR(card->ctx,
				SC_ERROR_INVALID_CARD,
				"APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	if (r < 0)
		LOG_TEST_GOTO_ERR(card->ctx,
				SC_ERROR_INVALID_CARD,
				"GET DATA command returned error");
	if (apdu.resplen != 2) {
		r = SC_ERROR_INVALID_CARD;
		goto err;
	}
	data_field_length = ((rbuf[0] << 8) | rbuf[1]);

	/* TODO is this really needed? strip the length of possible Lc and Le bytes */

	/* Use Min card sizes and reader too. for V5_3 at least*/

	if (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3) {
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "data_field_length:%"SC_FORMAT_LEN_SIZE_T"u "
				"card->reader->max_send_size:%"SC_FORMAT_LEN_SIZE_T"u "
				"card->reader->max_recv_size:%"SC_FORMAT_LEN_SIZE_T"u %s",
				data_field_length, card->reader->max_send_size, card->reader->max_recv_size,
				(card->caps & SC_CARD_CAP_APDU_EXT) ? "SC_CARD_CAP_APDU_EXT" : " ");

		if (card->caps & SC_CARD_CAP_APDU_EXT) {
			card->max_send_size  = data_field_length - 6;
#ifdef _WIN32
			/* Windows does not support PCSC PART_10 and may have forced reader to 255/256
			 * https://github.com/OpenSC/OpenSC/commit/eddea6f3c2d3dafc2c09eba6695c745a61b5186f
			 * may have reset this. if so, will override and force extended 
			 * Most, if not all, cardos cards do extended, but not chaining 
			 */
			if (card->reader->max_send_size == 255 && card->reader->max_recv_size == 256) {
				sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "resetting reader to use data_field_length");
				card->reader->max_send_size = data_field_length - 6;
				card->reader->max_recv_size = data_field_length - 3;
			}
#endif
		} else
			card->max_send_size = data_field_length - 3;

		card->max_send_size = sc_get_max_send_size(card); /* include reader sizes and protocol */
		card->max_recv_size = data_field_length - 2;
		card->max_recv_size = sc_get_max_recv_size(card);
	} else {
		/* old way, disregards reader capabilities */
		if (card->caps & SC_CARD_CAP_APDU_EXT)
			card->max_send_size = data_field_length - 6;
		else
			card->max_send_size = data_field_length - 3;
		/* strip the length of SW bytes */
		card->max_recv_size = data_field_length - 2;
	}

	/*for new cards, wait till after sc_pkcs15_bind_internal reads tokeninfo */
	if (card->type != SC_CARD_TYPE_CARDOS_V5_0 && card->type != SC_CARD_TYPE_CARDOS_V5_3) {
		r = cardos_add_algs(card, flags, 0, 0);
	}

err:
	if (r != SC_SUCCESS) {
		free(priv);
		card->drv_data = NULL;
	}

	return r;
}