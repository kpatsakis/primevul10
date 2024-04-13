static Jsi_RC fileObjFree(Jsi_Interp *interp, void *data)
{
    FileObj *fo = (FileObj *)data;
    SIGASSERT(fo,FILEOBJ);
    fileObjErase(fo);
    Jsi_Free(fo);
    return JSI_OK;
}