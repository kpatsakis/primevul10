void Parser::CheckPidIsValid(IdentPtr pid, bool autoArgumentsObject)
{
    if (IsStrictMode())
    {
        // in strict mode, variable named 'eval' cannot be created
        if (pid == wellKnownPropertyPids.eval)
        {
            Error(ERREvalUsage);
        }
        else if (pid == wellKnownPropertyPids.arguments && !autoArgumentsObject)
        {
            Error(ERRArgsUsage);
        }
    }
}