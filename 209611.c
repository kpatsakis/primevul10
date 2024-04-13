unsigned char *Jsi_ValueBlob(Jsi_Interp *interp, Jsi_Value* v, int *lenPtr)
{
    return (unsigned char*)Jsi_ValueString(interp, v, lenPtr);
}