oberthur_detect_card(struct sc_pkcs15_card * p15card)
{
	struct sc_card *card = p15card->card;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (p15card->card->type != SC_CARD_TYPE_OBERTHUR_64K)
		LOG_FUNC_RETURN(p15card->card->ctx, SC_ERROR_WRONG_CARD);
	LOG_FUNC_RETURN(p15card->card->ctx, SC_SUCCESS);
}