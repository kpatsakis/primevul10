extern void jsi_TypeMismatch(Jsi_Interp* interp)
{
    interp->typeMismatchCnt++;
    if (interp->typeWarnMax<=0)
        return;
    if (interp->typeMismatchCnt>=interp->typeWarnMax) {
        memset(&interp->typeCheck, 0, sizeof(interp->typeCheck));
        Jsi_LogWarn("Max warnings exceeded %d: typeCheck disabled", interp->typeWarnMax);
    }
}