static Jsi_RC jsi_InterpDelete(Jsi_Interp *interp, void *ptr) {
    if (jsi_vf)
        Jsi_DecrRefCount(interp, jsi_vf);
    jsi_vf = NULL;
    jsi_exitCode = interp->exitCode;
    jsi_deleted = 1;
    return JSI_OK;
}