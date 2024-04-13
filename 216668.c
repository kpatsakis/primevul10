cardos_decipher(struct sc_card *card,
		const u8 * crgram, size_t crgram_len,
		u8 * out, size_t outlen)
{
	cardos_data_t* priv = (cardos_data_t*)card->drv_data;
	int r;
	size_t card_max_send_size = card->max_send_size;
	size_t reader_max_send_size = card->reader->max_send_size;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* 5.3 supports command chaining. Others may also
	 * card_max_send_size for 5.3 is already based on reader max_send_size */

	if (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3) {

		r = iso_ops->decipher(card, crgram, crgram_len, out, outlen);
		/*
		 * 5.3 supports RAW as well as PKCS1 and PSS
		 * description may strip padding if card supports it
		 * with cards that support RAW, it always appears to 
		 * drop first 00 that is start of padding.
		 */

		if (r > 0 && priv->sec_env->algorithm_flags & SC_ALGORITHM_RSA_RAW) {
			size_t rsize = r;
			/* RSA RAW crgram_len == modlen */
			/* removed padding is always > 1 byte */
			/* add back missing leading zero if card dropped it */
			if (rsize == crgram_len - 1 && rsize < outlen) {
				memmove(out+1, out, rsize);
				out[0] =0x00;
				r++;
			}
		}

		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
	}

	if (sc_get_max_send_size(card) < crgram_len + 1) {
		/* CardOS doesn't support chaining for PSO:DEC, so we just _hope_
		 * that both, the reader and the card are able to send enough data.
		 * (data is prefixed with 1 byte padding content indicator) */
		card->max_send_size = crgram_len + 1;
		card->reader->max_send_size = crgram_len + 1;
	}

	r = iso_ops->decipher(card, crgram, crgram_len, out, outlen);

	/* reset whatever we've modified above */
	card->max_send_size = card_max_send_size;
	card->reader->max_send_size = reader_max_send_size;

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
}