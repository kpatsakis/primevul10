iasecc_keyset_change(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo_update update;
	struct iasecc_sdo sdo;
	unsigned scb;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Change keyset(ref:%i,lengths:%i)", data->pin_reference, data->pin2.len);
	if (!data->pin2.data || data->pin2.len < 32)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Needs at least 32 bytes for a new keyset value");

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_KEYSET;
	sdo.sdo_ref  = data->pin_reference;

	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_RET(ctx, rv, "Cannot get keyset data");

	if (sdo.docp.acls_contact.size == 0)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Bewildered ... there are no ACLs");
	scb = sdo.docp.scbs[IASECC_ACLS_KEYSET_PUT_DATA];
	iasecc_sdo_free_fields(card, &sdo);

	sc_log(ctx, "SCB:0x%X", scb);
	if (!(scb & IASECC_SCB_METHOD_SM))
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Other then protected by SM, the keyset change is not supported");

	memset(&update, 0, sizeof(update));
	update.magic = SC_CARDCTL_IASECC_SDO_MAGIC_PUT_DATA;
	update.sdo_class = sdo.sdo_class;
	update.sdo_ref = sdo.sdo_ref;

	update.fields[0].parent_tag = IASECC_SDO_KEYSET_TAG;
	update.fields[0].tag = IASECC_SDO_KEYSET_TAG_MAC;
	/* FIXME is it safe to modify the const value here? */
	update.fields[0].value = (unsigned char *) data->pin2.data;
	update.fields[0].size = 16;

	update.fields[1].parent_tag = IASECC_SDO_KEYSET_TAG;
	update.fields[1].tag = IASECC_SDO_KEYSET_TAG_ENC;
	/* FIXME is it safe to modify the const value here? */
	update.fields[1].value = (unsigned char *) data->pin2.data + 16;
	update.fields[1].size = 16;

	rv = iasecc_sm_sdo_update(card, (scb & IASECC_SCB_METHOD_MASK_REF), &update);
	LOG_FUNC_RETURN(ctx, rv);
}