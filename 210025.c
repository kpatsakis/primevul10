static char *get_inputline(Jsi_Interp *interp, int istty, const char *prompt)
{
    char *res;
#ifdef JSI_HAS_READLINE
    if (istty && interp->subOpts.noReadline==0) {
        res = jsi_sh_readline(prompt);
        if (res && *res) jsi_sh_add_history(res);
        return res;
    }
#endif
    int done = 0;
    char bbuf[JSI_BUFSIZ];
    Jsi_DString dStr = {};
    if (istty)
        fputs(prompt, stdout);
    fflush(stdout);
    while (!done) { /* Read a line. */
        bbuf[0] = 0;
        if (fgets(bbuf, sizeof(bbuf), stdin) == NULL)
            return NULL;
        Jsi_DSAppend(&dStr, bbuf, NULL);
        if (Jsi_Strlen(bbuf) < (sizeof(bbuf)-1) || bbuf[sizeof(bbuf)-1] == '\n')
            break;
    }
    res = Jsi_Strdup(Jsi_DSValue(&dStr));
    Jsi_DSFree(&dStr);
    return res;
}