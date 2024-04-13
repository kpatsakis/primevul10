static Jsi_Value *jsonNewStringObj(Jsi_Interp *interp, const char* str, int len)
{
    Jsi_Value *v = NULL;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    jsonNewDString(interp, &dStr, str, len);
    v = Jsi_ValueNew(interp);
    Jsi_ValueFromDS(interp, &dStr, &v);
    //Jsi_DSFree(&dStr);
    return v;
}