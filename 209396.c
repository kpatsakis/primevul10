static Jsi_RC ThreadEvalCallback(Jsi_Interp *interp, void* data) {
    Jsi_RC rc;

    if ((rc=SubInterpEvalCallback(interp, data)) != JSI_OK)
        interp->threadErrCnt++;
    return rc;
}