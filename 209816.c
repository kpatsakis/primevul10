static int dbGetIntBool(Jsi_Interp *interp, Jsi_Value* v)
{
    if (Jsi_ValueIsNumber(interp, v)) {
        Jsi_Number d;
        Jsi_ValueGetNumber(interp, v, &d);
        return (int)d;
    }
    if (Jsi_ValueIsBoolean(interp, v)) {
        bool n;
        Jsi_ValueGetBoolean(interp, v, &n);
        return n;
    }
    return 0;
}