char* Jsi_ValueGetStringLen(Jsi_Interp *interp, Jsi_Value *pv, int *lenPtr)
{
    if (!pv)
        return NULL;
    Jsi_String *s = jsi_ValueString(pv);
    if (!s)
        return NULL;
    if (lenPtr)
        *lenPtr = (s->len<0 ? (int)Jsi_Strlen(s->str) : s->len);
    return s->str;
}