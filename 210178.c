static Jsi_Filesystem *jsi_FilesysFind(const char *name)
{
    FSList *fsPtr = jsiFSList;
    while (fsPtr != NULL) {
        if (!Jsi_Strcmp(fsPtr->fsPtr->typeName, name))
            return fsPtr->fsPtr;
        fsPtr = fsPtr->next;
    }
    return NULL;
}