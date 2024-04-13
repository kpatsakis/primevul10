Jsi_RC jsi_InitNumber(Jsi_Interp *interp, int release)
{
    if (release) return JSI_OK;
    Jsi_Value *val, *global = interp->csc;
    val = interp->Number_prototype = Jsi_CommandCreateSpecs(interp, "Number", numberCmds, NULL, JSI_CMDSPEC_ISOBJ);

    Jsi_Value *NaN = Jsi_ValueMakeNumber(interp, NULL, Jsi_NumberNaN());

    Jsi_Value *Inf = Jsi_ValueMakeNumber(interp, NULL, Jsi_NumberInfinity(1));
    
    Jsi_ValueInsertFixed(interp, global, "NaN", NaN);
    Jsi_ValueInsertFixed(interp, global, "Infinity", Inf);
    interp->NaNValue = NaN;
    interp->InfValue = Inf;
#define MCONST(name,v) Jsi_ValueInsert(interp, val, name, Jsi_ValueNewNumber(interp, v), JSI_OM_READONLY)
    MCONST("MAX_VALUE", DBL_MAX);
    MCONST("MIN_VALUE", DBL_MIN);
    MCONST("NEGATIVE_INFINITY", Jsi_NumberInfinity(-1));
    Jsi_ValueInsertFixed(interp, val, "POSITIVE_INFINITY", Inf);
    Jsi_ValueInsertFixed(interp, val, "NaN", NaN);
    return JSI_OK;
}