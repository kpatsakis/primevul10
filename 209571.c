bool Jsi_ValueIsEqual(Jsi_Interp* interp, Jsi_Value* v1, Jsi_Value* v2)
{
    int eq = 0;
    if (v1->vt == JSI_VT_OBJECT && v2->vt == JSI_VT_OBJECT && v1->d.obj == v2->d.obj)
        eq = 1;
    else if (Jsi_ValueIsNull(interp, v1) && Jsi_ValueIsNull(interp, v2))
        eq = 1;
    else if (Jsi_ValueIsUndef(interp, v1) && Jsi_ValueIsUndef(interp, v2))
        eq = 1;
    else if (Jsi_ValueIsBoolean(interp, v1) && Jsi_ValueIsBoolean(interp, v2)) {
        bool b1, b2;
        eq = (Jsi_GetBoolFromValue(interp, v1, &b1) == JSI_OK
            && Jsi_GetBoolFromValue(interp, v2, &b2) == JSI_OK
            && b1 == b2);
    } else if (Jsi_ValueIsNumber(interp, v1) && Jsi_ValueIsNumber(interp, v2)) {
        Jsi_Number n1, n2;
        eq = (Jsi_GetNumberFromValue(interp, v1, &n1) == JSI_OK
            && Jsi_GetNumberFromValue(interp, v2, &n2) == JSI_OK
            && n1 == n2);
    } else if (Jsi_ValueIsString(interp, v1) && Jsi_ValueIsString(interp, v2)) {
        const char *s1, *s2;
        int l1, l2;
        eq = (((s1=Jsi_ValueString(interp, v1, &l1)) && ((s2=Jsi_ValueString(interp, v2, &l2)))
            && l1 == l2 && Jsi_Strcmp(s1, s2)==0));
    }
    return eq;
}