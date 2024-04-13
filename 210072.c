static void interpObjErase(InterpObj *fo)
{
    SIGASSERTV(fo,INTERPOBJ);
    if (fo->subinterp) {
        Jsi_Interp *interp = fo->subinterp;
        fo->subinterp = NULL;
        Jsi_InterpDelete(interp);
        /*fclose(fo->fp);
        Jsi_Free(fo->interpname);
        Jsi_Free(fo->mode);*/
    }
    fo->subinterp = NULL;
}