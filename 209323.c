Jsi_RC Jsi_DeleteData(Jsi_Interp* interp, void *m)
{
    Jsi_Free(m);
    return JSI_OK;
}