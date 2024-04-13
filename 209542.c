void Jsi_InterpDelete(Jsi_Interp* interp)
{
    if (interp->deleting || interp->level > 0 || !interp->onDeleteTbl)
        return;
    interp->deleting = 1;
    Jsi_HashDelete(interp->onDeleteTbl);
    interp->onDeleteTbl = NULL;
    Jsi_EventuallyFree(interp, interp, jsiInterpDelete);
}