static int codes_insertln(Jsi_OpCodes *c, jsi_Eopcode code, void *extra, jsi_Pstate *pstate, jsi_Pline *line, int doalloc)
{
    if (c->code_size - c->code_len <= 0) {
        c->code_size += 100;
        c->codes = (jsi_OpCode *)Jsi_Realloc(c->codes, c->code_size * sizeof(jsi_OpCode));
    }
    c->codes[c->code_len].op = code;
    c->codes[c->code_len].data = extra;
    c->codes[c->code_len].Line = line->first_line;
    c->codes[c->code_len].Lofs = line->first_column;
    c->codes[c->code_len].fname = jsi_PstateGetFilename(pstate);
    c->codes[c->code_len].alloc = doalloc;
    c->code_len ++;
    return 0;
}