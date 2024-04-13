static Jsi_OpCodes *opassign(jsi_Pstate *pstate, jsi_Pline *line, Jsi_OpCodes *lval, Jsi_OpCodes *oprand, Jsi_OpCodes *op)
{
    Jsi_OpCodes *ret;
    if ((lval)->lvalue_flag == 1) {
        ret = codes_join3(lval, 
                             codes_join3(code_push_top(), oprand, op),
                             code_assign(pstate, line, 1));
    } else {
        ret = codes_join3(lval,
                             codes_join4(code_push_top2(), code_subscript(pstate, line, 1), oprand, op),
                             code_assign(pstate, line, 2));
    }
    return ret;
}