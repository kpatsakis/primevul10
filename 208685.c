void Parser::CheckArgumentsUse(IdentPtr pid, ParseNodePtr pnodeFnc)
{
    if (pid == wellKnownPropertyPids.arguments)
    {
        if (pnodeFnc != nullptr && pnodeFnc != m_currentNodeProg)
        {
            pnodeFnc->sxFnc.SetUsesArguments(TRUE);
        }
        else
        {
            m_UsesArgumentsAtGlobal = true;
        }
    }
}