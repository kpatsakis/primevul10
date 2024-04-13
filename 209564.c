Jsi_RC Jsi_FuncObjToString(Jsi_Interp *interp, Jsi_Func *f, Jsi_DString *dStr, int flags)
{
    int withBody = flags&1;
    int withTypes = flags&2;
    int withJSON = flags&4;
    int withFull = (flags&8 && !withJSON);
    if (withFull && f->type == FC_NORMAL && f->opcodes) {
        int len;
        const char *cp = jsi_FuncGetCode(interp, f, &len);
        if (cp) {
            Jsi_DSAppendLen(dStr,cp, len);
            return JSI_OK;
        }
    }
    Jsi_CmdSpec *spec = f->cmdSpec;
    if (withJSON)
        Jsi_DSAppend(dStr, "\"", NULL);
    if (f->type == FC_NORMAL) {
        Jsi_DSAppend(dStr, "function ", f->name?f->name:"", "(", NULL);
        jsi_FuncArgsToString(interp, f, dStr, withTypes);
        Jsi_DSAppend(dStr, ")", NULL);
        if (withTypes && f->retType) {
            Jsi_DString tStr;
            Jsi_DSInit(&tStr);
            Jsi_DSAppend(dStr, ":", jsi_typeName(interp, f->retType, &tStr), NULL);
            Jsi_DSFree(&tStr);
        }
        if (withBody)
            Jsi_DSAppend(dStr, " {...}", NULL);
    } else {
        Jsi_DSAppend(dStr, "function ", f->name?f->name:"", "(",
            (spec&&spec->argStr)?spec->argStr:"", ")", NULL);
        if (withBody)
            Jsi_DSAppend(dStr, " { [native code] }", NULL);
    }
    if (withJSON)
        Jsi_DSAppend(dStr, "\"", NULL);
    return JSI_OK;
}