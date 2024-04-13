void PrintBlockType(PnodeBlockType type)
{
    switch (type)
    {
    case Global:
        Output::Print(_u("(Global)"));
        break;
    case Function:
        Output::Print(_u("(Function)"));
        break;
    case Regular:
        Output::Print(_u("(Regular)"));
        break;
    case Parameter:
        Output::Print(_u("(Parameter)"));
        break;
    default:
        Output::Print(_u("(unknown blocktype)"));
        break;
    }
}