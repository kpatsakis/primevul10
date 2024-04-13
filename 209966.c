static Jsi_OpCodes *make_forin(Jsi_OpCodes *lval, jsi_Pline *line, Jsi_OpCodes *expr, Jsi_OpCodes *stat, const char *label, int isof)
{
    Jsi_OpCodes *keycode = code_key();
    keycode->codes[0].isof = isof;
    keycode->codes[0].Line = line->first_line;
    Jsi_OpCodes *init = codes_join(expr, keycode);
    Jsi_OpCodes *cond = codes_join3(lval, code_next(),
                                   code_jfalse(stat->code_len + 2));
    Jsi_OpCodes *stat_jmp = code_jmp(-(cond->code_len + stat->code_len));
    code_reserved_replace(stat, 1, 0, label, 2);
    return codes_join3(codes_join(init, cond), 
                          codes_join(stat, stat_jmp), code_pop(2));
}