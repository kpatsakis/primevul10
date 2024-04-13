static Jsi_RC jsi_ValueAssignCheck(Jsi_Interp *interp, Jsi_Value *val, int lop) {
    if (lop == OP_FCALL || lop == OP_NEWFCALL)
        return JSI_OK;
    if (val->f.bits.lookupfailed && val->d.lookupFail)
        return Jsi_LogType("assign from undef var: %s", val->d.lookupFail);
    return JSI_OK;
}