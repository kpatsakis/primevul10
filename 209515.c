static void jsiSetupStack(Jsi_Interp *interp)
{
    int oldsz = interp->maxStack;
    if (interp->maxStack)
        interp->maxStack += STACK_INCR_SIZE;
    else
        interp->maxStack = STACK_INIT_SIZE;
    _jsi_STACK = jsi_ValuesAlloc(interp, interp->maxStack, _jsi_STACK, oldsz);
    _jsi_THIS = jsi_ValuesAlloc(interp, interp->maxStack, _jsi_THIS, oldsz); //TODO:!!! use interp->framePtr for this.
}