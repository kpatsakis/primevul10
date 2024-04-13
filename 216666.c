static int cardos_select_file(sc_card_t *card,
			      const sc_path_t *in_path,
			      sc_file_t **file)
{
	int r;
	
	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	r = iso_ops->select_file(card, in_path, file);
	if (r >= 0 && file)
		parse_sec_attr((*file), (*file)->sec_attr, (*file)->sec_attr_len);
	LOG_FUNC_RETURN(card->ctx, r);
}