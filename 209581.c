void *Jsi_StackUnshift(Jsi_Stack *stack)
{
    if (stack->len == 0)
        return NULL;
    stack->len--;
    void *rc = stack->vector[0];
    memmove(stack->vector, stack->vector+1, sizeof(void*)*stack->len);
    return rc;
}