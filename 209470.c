static Jsi_OpCodes *code_push_string(jsi_Pstate *p, jsi_Pline *line, const char *str) {
    if (*str == 'c' && !Jsi_Strcmp(str,"callee"))
        p->interp->hasCallee = 1;
    JSI_NEW_CODESLN(0,OP_PUSHSTR, str);
}