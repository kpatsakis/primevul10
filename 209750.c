static void fileObjErase(FileObj *fo)
{
    if (fo->filename) {
        Jsi_Close(fo->interp, fo->chan);
        Jsi_Free(fo->filename);
        Jsi_DecrRefCount(fo->interp, fo->fname);
        Jsi_Free(fo->mode);
    }
    fo->filename = NULL;
}