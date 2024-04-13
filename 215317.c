iasecc_pin_get_policy (struct sc_card *card, struct sc_pin_cmd_data *data, struct iasecc_pin_policy *pin)
{
	struct sc_context *ctx = card->ctx;
	struct sc_file *save_current_df = NULL, *save_current_ef = NULL;
	struct iasecc_sdo sdo;
	struct sc_path path;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_pin_get_policy(card:%p)", card);

	if (data->pin_type != SC_AC_CHV)   {
		sc_log(ctx, "PIN policy only available for CHV type");
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);
	}

	if (card->cache.valid && card->cache.current_df)   {
		sc_file_dup(&save_current_df, card->cache.current_df);
		if (save_current_df == NULL) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			sc_log(ctx, "Cannot duplicate current DF file");
			goto err;
		}
	}

	if (card->cache.valid && card->cache.current_ef)   {
		sc_file_dup(&save_current_ef, card->cache.current_ef);
		if (save_current_ef == NULL) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			sc_log(ctx, "Cannot duplicate current EF file");
			goto err;
		}
	}

	if (!(data->pin_reference & IASECC_OBJECT_REF_LOCAL) && card->cache.valid && card->cache.current_df) {
		sc_format_path("3F00", &path);
		path.type = SC_PATH_TYPE_FILE_ID;
		rv = iasecc_select_file(card, &path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Unable to select MF");
	}

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_CHV;

	sdo.sdo_ref = data->pin_reference & ~IASECC_OBJECT_REF_LOCAL;

	sc_log(ctx, "iasecc_pin_get_policy() reference %i", sdo.sdo_ref);

	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_GOTO_ERR(ctx, rv, "Cannot get SDO PIN data");

	if (sdo.docp.acls_contact.size == 0) {
		rv = SC_ERROR_INVALID_DATA;
		sc_log(ctx, "Extremely strange ... there is no ACLs");
		goto err;
	}

	sc_log(ctx,
	       "iasecc_pin_get_policy() sdo.docp.size.size %"SC_FORMAT_LEN_SIZE_T"u",
	       sdo.docp.size.size);

	memcpy(pin->scbs, sdo.docp.scbs, sizeof(pin->scbs));

	pin->min_length = (sdo.data.chv.size_min.value ? *sdo.data.chv.size_min.value : -1);
	pin->max_length = (sdo.data.chv.size_max.value ? *sdo.data.chv.size_max.value : -1);
	pin->tries_maximum = (sdo.docp.tries_maximum.value ? *sdo.docp.tries_maximum.value : -1);
	pin->tries_remaining = (sdo.docp.tries_remaining.value ? *sdo.docp.tries_remaining.value : -1);
	if (sdo.docp.size.value && sdo.docp.size.size <= sizeof(int)) {
		unsigned int n = 0;
		unsigned int i;
		for (i=0; i<sdo.docp.size.size; i++)
			n = (n << 8) + *(sdo.docp.size.value + i);
		pin->stored_length = n;
	} else {
		pin->stored_length = -1;
	}

	sc_log(ctx, "PIN policy: size max/min %i/%i, tries max/left %i/%i",
	       pin->max_length, pin->min_length, pin->tries_maximum, pin->tries_remaining);
	iasecc_sdo_free_fields(card, &sdo);

	if (save_current_df)   {
		sc_log(ctx, "iasecc_pin_get_policy() restore current DF");
		rv = iasecc_select_file(card, &save_current_df->path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Cannot return to saved DF");
	}

	if (save_current_ef)   {
		sc_log(ctx, "iasecc_pin_get_policy() restore current EF");
		rv = iasecc_select_file(card, &save_current_ef->path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Cannot return to saved EF");
	}

err:
	sc_file_free(save_current_df);
	sc_file_free(save_current_ef);

	LOG_FUNC_RETURN(ctx, rv);
}