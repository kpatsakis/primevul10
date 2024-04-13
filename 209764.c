static Jsi_RC interpObjFree(Jsi_Interp *interp, void *data)
{
    InterpObj *fo = (InterpObj *)data;
    SIGASSERT(fo,INTERPOBJ);
    if (fo->deleting) return JSI_OK;
    fo->deleting = 1;
    interpObjErase(fo);
    Jsi_Free(fo);
    return JSI_OK;
}