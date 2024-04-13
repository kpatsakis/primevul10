void jsi_FilesysDone(Jsi_Interp *interp)
{
    Jsi_UserObjUnregister(interp, &fileobject);
    Jsi_InitZvfs(interp, 1);
    while (jsiFSList != NULL) {
        FSList *fsPtr = jsiFSList;
        jsiFSList = fsPtr->next;
        if (fsPtr)
            Jsi_Free(fsPtr);
    }
}