static Jsi_OpCodes *code_subscript(jsi_Pstate *p, jsi_Pline *line, int right_val) { JSI_NEW_CODESLN(0,OP_SUBSCRIPT, right_val); }