static void jsi_ParentFuncFree(Jsi_Interp *interp, Jsi_OptionSpec* spec, void *ptr)
{
    Jsi_Value **v = (Jsi_Value **)(((char*)ptr));
    if (!interp->parent) return;
    if (*v)
        Jsi_DecrRefCount(interp->parent, *v);
    *v = NULL;
}