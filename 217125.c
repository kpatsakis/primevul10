get_complete_funcname(int type)
{
    switch (type)
    {
	case CTRL_X_FUNCTION:
	    return curbuf->b_p_cfu;
	case CTRL_X_OMNI:
	    return curbuf->b_p_ofu;
	case CTRL_X_THESAURUS:
	    return *curbuf->b_p_tsrfu == NUL ? p_tsrfu : curbuf->b_p_tsrfu;
	default:
	    return (char_u *)"";
    }
}