bool Jsi_InterpGone( Jsi_Interp* interp)
{
    return (interp == NULL || interp->deleting || interp->destroying || interp->exited);
}