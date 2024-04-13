static int cardos_pass_algo_flags(sc_card_t *card, struct sc_cardctl_cardos_pass_algo_flags * ptr)
{
	cardos_data_t * priv = (cardos_data_t *)card->drv_data;
	int r = 0;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	switch (ptr->pass) {
		case 1:
			ptr->card_flags = card->flags;
			ptr->used_flags = priv->flags;
			ptr->ec_flags = priv->ec_flags;
			ptr->ext_flags = priv->ext_flags;
			break;
		case 2:
			r = cardos_add_algs(card,ptr->new_flags, ptr->ec_flags, ptr->ext_flags);
			break;
		default:
			sc_log(card->ctx, "ptr->pass: %ul invalid", ptr->pass);
			r = SC_ERROR_INTERNAL;
	}
	LOG_FUNC_RETURN(card->ctx, r);
}