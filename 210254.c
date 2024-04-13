char *Jsi_ValueArrayIndexToStr(Jsi_Interp *interp, Jsi_Value *args, int index, int *lenPtr)
{
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, index);
    if (!arg)
        return NULL;
    char *res = Jsi_ValueString(interp, arg, lenPtr);
    if (res)
        return res;
    res = (char*)Jsi_ValueToString(interp, arg, NULL);
    if (res && lenPtr)
        *lenPtr = Jsi_Strlen(res);
    return res;
}