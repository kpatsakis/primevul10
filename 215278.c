iasecc_chv_cache_clean(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_pin_status *current = NULL;

	LOG_FUNC_CALLED(ctx);

	for(current = checked_pins; current; current = current->next)
		if (current->reference == pin_cmd->pin_reference)
			break;

	if (!current)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);


	if (current->next && current->prev)   {
		current->prev->next = current->next;
		current->next->prev = current->prev;
	}
	else if (!current->prev)   {
		checked_pins = current->next;
	}
	else if (!current->next && current->prev)   {
		current->prev->next = NULL;
	}

	free(current);
	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}