void Parser::UpdateOrCheckForDuplicateInFormals(IdentPtr pid, SList<IdentPtr> *formals)
{
    bool isStrictMode = IsStrictMode();
    if (isStrictMode)
    {
        CheckStrictModeEvalArgumentsUsage(pid);
    }

    if (formals->Has(pid))
    {
        if (isStrictMode)
        {
            Error(ERRES5ArgSame);
        }
        else
        {
            Error(ERRFormalSame);
        }
    }
    else
    {
        formals->Prepend(pid);
    }
}