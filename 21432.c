re_mult_next(char *what)
{
    if (re_multi_type(peekchr()) == MULTI_MULT)
    {
       semsg(_("E888: (NFA regexp) cannot repeat %s"), what);
       rc_did_emsg = TRUE;
       return FAIL;
    }
    return OK;
}