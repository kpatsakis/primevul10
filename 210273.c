static Jsi_OpCodes *code_eval(jsi_Pstate *p, jsi_Pline *line, int argc, Jsi_OpCodes *c) {
    jsi_FreeOpcodes(c); // Eliminate leak of unused opcodes.
    JSI_NEW_CODESLN(0,OP_EVAL, argc);
}