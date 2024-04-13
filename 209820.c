void jsi_PstateClear(jsi_Pstate *ps)
{
    jsi_Lexer* l = ps->lexer;
    if (l->ltype == LT_FILE)
    {
        if (l->d.fp)
            Jsi_Close(ps->interp, l->d.fp);
        l->d.fp = NULL;
    }
    if (l->ltype == LT_STRING)
    {
        l->d.str = NULL;
    }
    l->ltype = LT_NONE;
    l->last_token = 0;
    l->cur_line = 1;
    l->cur_char = 0;
    l->cur = 0;
    ps->err_count = 0;
}