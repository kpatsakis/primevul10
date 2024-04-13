int jsi_PstateSetFile(jsi_Pstate *ps, Jsi_Channel fp, int skipbang)
{
    jsi_Lexer *l = ps->lexer;
    jsi_PstateClear(ps);
    l->ltype = LT_FILE;
    l->d.fp = fp;
    Jsi_Rewind(ps->interp, fp);
    if (skipbang) {
        char buf[JSI_BUFSIZ];
        if (Jsi_Gets(ps->interp, fp, buf, sizeof(buf)) && (buf[0] != '#' || buf[1] != '!')) {
            Jsi_Rewind(ps->interp, fp);
        }
    }
            
    return JSI_OK;
}