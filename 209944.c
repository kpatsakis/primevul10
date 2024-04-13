static bool fileObjIsTrue(void *data)
{
    FileObj *fo = (FileObj *)data;
    SIGASSERT(fo,FILEOBJ);
    if (!fo->filename) return JSI_OK;
    else return 1;
}