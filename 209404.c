static Jsi_Value** jsi_ValuesAlloc(Jsi_Interp *interp, int cnt, Jsi_Value**old, int oldsz) {
    int i;
    Jsi_Value **v = (Jsi_Value **)Jsi_Realloc(old, cnt* sizeof(Jsi_Value*));
    for (i=oldsz; i<cnt; i++)
        v[i] = NULL;
    return v;
}