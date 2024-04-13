static Jsi_Value *ValueNew1(Jsi_Interp *interp)
{
    Jsi_Value *v = ValueNew(interp);
    Jsi_IncrRefCount(interp, v);
    return v;
}