void Jsi_ValueReplace(Jsi_Interp *interp, Jsi_Value **to, Jsi_Value *from )  {
    VALCHK(from);
    if( *to == from) return;
    if (*to)
        Jsi_DecrRefCount(interp, *to);
    *to = from;
    if (from)
        Jsi_IncrRefCount(interp, from);
}