static void jsiDumpInstr(Jsi_Interp *interp, jsi_Pstate *ps, Jsi_Value *_this,
    jsi_TryList *trylist, jsi_OpCode *ip, Jsi_OpCodes *opcodes)
{
    int i;
    char buf[JSI_MAX_NUMBER_STRING*2];
    jsi_code_decode(interp, ip, ip - opcodes->codes, buf, sizeof(buf));
    Jsi_Printf(interp, jsi_Stderr, "%p: %-30.200s : THIS=%s, STACK=[", ip, buf, jsi_evalprint(_this));
    for (i = 0; i < interp->framePtr->Sp; ++i) {
        Jsi_Printf(interp, jsi_Stderr, "%s%s", (i>0?", ":""), jsi_evalprint(_jsi_STACKIDX(i)));
    }
    Jsi_Printf(interp, jsi_Stderr, "]");
    if (ip->fname) {
        const char *fn = ip->fname,  *cp = Jsi_Strrchr(fn, '/');
        if (cp) fn = cp+1;
        Jsi_Printf(interp, jsi_Stderr, ", %s:%d", fn, ip->Line);
    }
    Jsi_Printf(interp, jsi_Stderr, "\n");
    jsi_TryList *tlt = trylist;
    for (i = 0; tlt; tlt = tlt->next) i++;
    if (ps->last_exception)
        Jsi_Printf(interp, jsi_Stderr, "TL: %d, excpt: %s\n", i, jsi_evalprint(ps->last_exception));
}