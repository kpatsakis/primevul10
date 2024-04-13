static jsi_ForinVar *forinvar_new(jsi_Pstate *pstate, const char *varname, Jsi_OpCodes *local, Jsi_OpCodes *lval)
{
    jsi_ForinVar *r = (jsi_ForinVar*)Jsi_Calloc(1,sizeof(*r));
    r->sig = JSI_SIG_FORINVAR;
    r->varname = varname;
    r->local = local;
    r->lval = lval;
    return r;
}