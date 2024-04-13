static Jsi_OpCodes *code__fcall(jsi_Pstate *p, jsi_Pline *line, int argc, const char *name, const char *namePre, Jsi_OpCodes *argCodes) {
    jsi_FuncCallCheck(p,line,argc,1, name, namePre, argCodes);JSI_NEW_CODESLN(0,OP_FCALL, argc);
}