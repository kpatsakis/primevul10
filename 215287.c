iasecc_finish(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_private_data *private_data = (struct iasecc_private_data *)card->drv_data;
	struct iasecc_se_info *se_info = private_data->se_info, *next;

	LOG_FUNC_CALLED(ctx);

	while (se_info)   {
		sc_file_free(se_info->df);
		next = se_info->next;
		free(se_info);
		se_info = next;
	}

	free(card->drv_data);
	card->drv_data = NULL;

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}