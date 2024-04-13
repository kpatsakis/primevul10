void Jsi_StackFreeElements(Jsi_Interp *interp, Jsi_Stack *stack, Jsi_DeleteProc *freeProc)
{
    int i;
    for (i = 0; i < stack->len; i++)
        freeProc(interp, stack->vector[i]);
    stack->len = 0;
}