static Jsi_OpCodes *code_newfcall(jsi_Pstate *p, jsi_Pline *line, int argc, const char *name, Jsi_OpCodes *argCodes) {
    jsi_FuncCallCheck(p,line,argc,1, name, NULL, argCodes); JSI_NEW_CODESLN(0,OP_NEWFCALL, argc);
}