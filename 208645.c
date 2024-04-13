void Parser::CheckStrictModeEvalArgumentsUsage(IdentPtr pid, ParseNodePtr pnode)
{
    if (pid != nullptr)
    {
        // In strict mode, 'eval' / 'arguments' cannot be assigned to.
        if ( pid == wellKnownPropertyPids.eval)
        {
            Error(ERREvalUsage, pnode);
        }

        if (pid == wellKnownPropertyPids.arguments)
        {
            Error(ERRArgsUsage, pnode);
        }
    }
}