static Jsi_OpCodes *code_push_func(jsi_Pstate *p, jsi_Pline *line, Jsi_Func *fun) {
    Jsi_OpCodes* codes = code_push_func_sub(p, line, fun);
    if (codes && fun && fun->name)
        codes->codes[0].local = 1;
    return codes;
}