int Jsi_ValueGetLength(Jsi_Interp *interp, Jsi_Value *v) {
    if (Jsi_ValueIsArray(interp, v))
        return v->d.obj->arrCnt;
    Jsi_LogWarn("expected array");
    return 0;
}