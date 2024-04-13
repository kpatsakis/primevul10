static Jsi_OpCodes *code_push_vstring(jsi_Pstate *p, jsi_Pline *line, Jsi_String *s) {
    JSI_NEW_CODESLN(0,OP_PUSHVSTR, s);
}