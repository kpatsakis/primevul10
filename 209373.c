Jsi_RC jsi_InitFilesys(Jsi_Interp *interp, int release)
{
    if (release) {
        jsi_FilesysDone(interp);
        return JSI_OK;
    }
    Jsi_DSInit(&jsiIntData.pwdStr);
    Jsi_Hash *fsys = Jsi_UserObjRegister(interp, &fileobject);
    if (!fsys)
        return Jsi_LogBug("Can not init file system");
    Jsi_CommandCreateSpecs(interp, fileobject.name, filesysCmds, fsys, JSI_CMDSPEC_ISOBJ);
    if (jsiFSList == NULL) {
        Jsi_FSRegister(&jsiFilesystem, NULL);
        SetupStdChan(jsiIntData.stdChans, stdin, &jsiFilesystem, JSI_FS_READONLY);
        SetupStdChan(jsiIntData.stdChans+1, stdout, &jsiFilesystem, JSI_FS_WRITEONLY);
        SetupStdChan(jsiIntData.stdChans+2, stderr, &jsiFilesystem, JSI_FS_WRITEONLY);
    }
#if JSI__ZVFS==1
    Jsi_InitZvfs(interp, 0);
#endif
#ifndef JSI_OMIT_VFS
    jsi_InitVfs(interp, 0);
#endif
    return JSI_OK;
}