iasecc_chv_verify(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd, unsigned char *scbs,
		  int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	unsigned char scb = scbs[IASECC_ACLS_CHV_VERIFY];
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Verify CHV PIN(ref:%i,len:%i,scb:%X)", pin_cmd->pin_reference, pin_cmd->pin1.len,
	       scb);

	if (scb & IASECC_SCB_METHOD_SM) {
		rv = iasecc_sm_pin_verify(card, scb & IASECC_SCB_METHOD_MASK_REF, pin_cmd, tries_left);
		LOG_FUNC_RETURN(ctx, rv);
	}

	rv = iso_ops->pin_cmd(card, pin_cmd, tries_left);
	LOG_FUNC_RETURN(ctx, rv);
}