static Jsi_Value *ValueDup(Jsi_Interp *interp, Jsi_Value *v)
{
    Jsi_Value *r = ValueNew1(interp);
    Jsi_ValueCopy(interp,r, v);
#ifdef JSI_MEM_DEBUG
    r->VD.label2 = "ValueDup";
#endif
    return r;
}