iasecc_match_card(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	int i;

	i = _sc_match_atr(card, iasecc_known_atrs, &card->type);
	if (i < 0)   {
		sc_log(ctx, "card not matched");
		return 0;
	}

	sc_log(ctx, "'%s' card matched", iasecc_known_atrs[i].name);
	return 1;
}