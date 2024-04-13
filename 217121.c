get_insert_callback(int type)
{
    if (type == CTRL_X_FUNCTION)
	return &curbuf->b_cfu_cb;
    if (type == CTRL_X_OMNI)
	return &curbuf->b_ofu_cb;
    // CTRL_X_THESAURUS
    return (*curbuf->b_p_tsrfu != NUL) ? &curbuf->b_tsrfu_cb : &tsrfu_cb;
}