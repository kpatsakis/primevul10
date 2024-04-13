char *Jsi_ValueString(Jsi_Interp *interp, Jsi_Value* v, int *lenPtr)
{
    if (!v) return NULL;
    Jsi_String *s = jsi_ValueString(v);
    if (s) {
        if (lenPtr)
            *lenPtr = (s->len<0 ? (int)Jsi_Strlen(s->str) : s->len);
        return s->str;
    }
    if (lenPtr)
        *lenPtr = 0;
    return NULL;
}