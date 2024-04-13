bool Jsi_FunctionIsConstructor(Jsi_Func *funcPtr)
{
    return (funcPtr->f.bits.iscons);
}