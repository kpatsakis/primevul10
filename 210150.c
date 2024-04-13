void *Jsi_StackHead(Jsi_Stack *stack)
{
    if (stack->len == 0)
        return NULL;
    return stack->vector[0];
}