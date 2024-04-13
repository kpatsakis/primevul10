char *Jsi_FileRealpath(Jsi_Interp *interp, Jsi_Value *spath, char *newname)
{
    char *path = Jsi_ValueString(interp, spath, 0);
    if (!path) return NULL;
    return Jsi_FileRealpathStr(interp, path, newname);
}