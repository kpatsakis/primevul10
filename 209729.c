static void jsi_VerifyFree(Jsi_Interp *interp, Jsi_OptionSpec* spec, void *ptr)
{
    Jsi_Value **v = (Jsi_Value**)ptr;
    if (v && *v)
        Jsi_DecrRefCount(interp, *v);
}