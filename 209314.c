static Jsi_OpCodes *code_push_regex(jsi_Pstate *p, jsi_Pline *line, Jsi_Regex *reg) { JSI_NEW_CODESLN(0,OP_PUSHREG, reg); }