Jsi_Stack* Jsi_StackNew(void)
{
    Jsi_Stack *stack = (Jsi_Stack*)Jsi_Calloc(1, sizeof(Jsi_Stack));
    return stack;
}