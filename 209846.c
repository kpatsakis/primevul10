jsonGenObject(Jsi_Interp *interp, Jsi_JsonParser *p, const char *js, uint pos, uint *endPos)
{
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
    Jsi_Value *nv, *v = Jsi_ValueMakeObject(interp, NULL, obj);
    uint i, n, len;
    Jsi_JsonTok *tok;
    const char *t;
    tok = p->tokens+pos;
    for (i=pos+1, n=0; i<p->toknext && n<tok->size; i++, n++) {

        Jsi_DString dStr;
        t = Jsi_JsonGetTokstr(p, js, i, &len);
        i++; n++;
        if (n>=tok->size)
            nv = Jsi_ValueMakeUndef(interp, NULL);
        else
            nv = jsonGen1Value(interp, p, js, i, &i, 0);
        Jsi_DSInit(&dStr);
        Jsi_DSAppendLen(&dStr, t, len);
        Jsi_ObjInsert(interp, obj, Jsi_DSValue(&dStr), nv, 0);
        Jsi_DSFree(&dStr);
    }
    if (endPos)
        *endPos = i;
    return v;
}