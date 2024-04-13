int Jsi_Scandir(Jsi_Interp *interp, Jsi_Value* path, Jsi_Dirent ***namelist,
 int (*filter)(const Jsi_Dirent *), int (*compar)(const Jsi_Dirent **, const Jsi_Dirent**)) 
 {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->scandirProc) return -1;
    return fsPtr->scandirProc(interp, path, namelist, filter, compar);
}