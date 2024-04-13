void Jsi_StackFree(Jsi_Stack *stack)
{
    Jsi_Free(stack->vector);
    Jsi_Free(stack);
}