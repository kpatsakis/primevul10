static Jsi_OpCodes *code_push_index(jsi_Pstate *p, jsi_Pline *line, const char *varname, int local)
{
    jsi_FastVar *n = (typeof(n))Jsi_Calloc(1, sizeof(*n));
    n->sig = JSI_SIG_FASTVAR;
    n->ps = p;
    n->context_id = -1;
    n->local = local;
    n->varname = (char*)Jsi_KeyAdd(p->interp, varname);
    Jsi_HashSet(p->fastVarTbl, n, n);
    JSI_NEW_CODESLN(1,OP_PUSHVAR, n);
}