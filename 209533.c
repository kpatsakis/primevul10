void Jsi_ValueDup2(Jsi_Interp *interp, Jsi_Value **to, Jsi_Value *from )
{
    if (!*to)
        *to = Jsi_ValueNew1(interp);
#ifdef JSI_MEM_DEBUG
    (*to)->VD.label3 = "ValueDup2";
#endif
    Jsi_ValueCopy(interp, *to, from);
    (*to)->f.bits.readonly = 0;
}