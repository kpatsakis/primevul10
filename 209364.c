static Jsi_RC jsi_StringFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *d) {
    Jsi_String *s = (Jsi_String *)d;
    if (s->flags&1)
        Jsi_Free(s->str);
    Jsi_Free(s);
    return JSI_OK;
}