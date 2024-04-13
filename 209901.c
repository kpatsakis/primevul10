int jsi_ValueToOInt32(Jsi_Interp *interp, Jsi_Value *v)
{
    Jsi_Number a = Jsi_ValueToNumberInt(interp, v, 1);
    Jsi_ValueReset(interp,&v);
    Jsi_ValueMakeNumber(interp, &v, a);
    return (int)a;
}