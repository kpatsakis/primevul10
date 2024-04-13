static int try_open_file(Jsi_Interp *interp, FileObj *udf, Jsi_Value *args)
{
    int ret = JSI_ERROR;
    fileObjErase(udf);
    // TODO: stdin, stdout, stderr, etc.
    Jsi_Value *fname = Jsi_ValueArrayIndex(interp, args, 0);
    if (fname && Jsi_ValueIsString(interp, fname)) {
        Jsi_Value *vmode = Jsi_ValueArrayIndex(interp, args, 1);
        const char *mode = NULL;
        const char *fstr = Jsi_ValueString(interp, fname, NULL);
        if (vmode && Jsi_ValueIsString(interp,vmode)) {
            mode = Jsi_ValueString(interp, vmode, NULL);
        }
        int writ = (mode && (Jsi_Strchr(mode,'w')||Jsi_Strchr(mode,'+')));
        int aflag = (writ ? JSI_INTACCESS_WRITE : JSI_INTACCESS_READ);
        if (interp->isSafe && Jsi_InterpAccess(interp, fname, aflag) != JSI_OK)
            return Jsi_LogError("Safe open failed");
        char *rmode = Jsi_Strdup(mode ? mode : "r");
        Jsi_Channel chan = Jsi_Open(interp, fname, rmode);
        if (chan) {
            udf->chan = chan;
            udf->fname = fname;
            udf->interp = interp;
            Jsi_IncrRefCount(interp, fname);
            udf->filename = Jsi_Strdup(fstr);
            udf->mode = Jsi_Strdup(rmode);
            ret = JSI_OK;
        }
        Jsi_Free(rmode);
    }
    return ret;
}