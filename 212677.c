sc_pkcs15emu_oberthur_init_ex(struct sc_pkcs15_card * p15card, struct sc_aid *aid)
{
	int rv;

	LOG_FUNC_CALLED(p15card->card->ctx);
	rv = oberthur_detect_card(p15card);
	if (!rv)
		rv = sc_pkcs15emu_oberthur_init(p15card);

	LOG_FUNC_RETURN(p15card->card->ctx, rv);
}