jsonGenArray(Jsi_Interp *interp, Jsi_JsonParser *p, const char *js, uint pos, uint *endPos)
{
    if (js==NULL || !js[0])
        return NULL;
    Jsi_Value *v = Jsi_ValueNewArray(interp, 0, 0);
    Jsi_Obj *nobj = v->d.obj;
    uint i, n;
    Jsi_JsonTok *tok;
    
    tok = p->tokens+pos;
    Jsi_ValueMakeArrayObject(interp, &v, nobj);
    for (i=pos+1, n=0; i<p->toknext && n<tok->size; i++, n++) {
        if (n >= nobj->arrMaxSize) {
            if (Jsi_ObjArraySizer(interp, nobj, n+1)<=0) {
                break;
            }
        }
        nobj->arr[n] = jsonGen1Value(interp, p, js, i, &i, 1);
    }
    Jsi_ObjSetLength(interp, nobj, n);
    if (endPos)
        *endPos = i;
    return v;
}