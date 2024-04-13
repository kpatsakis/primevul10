thesaurus_func_complete(int type UNUSED)
{
#ifdef FEAT_COMPL_FUNC
    return type == CTRL_X_THESAURUS
		&& (*curbuf->b_p_tsrfu != NUL || *p_tsrfu != NUL);
#else
    return FALSE;
#endif
}