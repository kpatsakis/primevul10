bool Jsi_ValueIsType(Jsi_Interp *interp, Jsi_Value *pv, Jsi_vtype vtype) {
    if (!pv) return 0;
    return pv->vt == vtype;
}