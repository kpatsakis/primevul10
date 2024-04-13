void Parser::UpdateCurrentNodeFunc(ParseNodePtr pnodeFnc, bool fLambda)
{
    if (buildAST)
    {
        // Make this the current function and start its sub-function list.
        m_currentNodeFunc = pnodeFnc;

        Assert(m_currentNodeDeferredFunc == nullptr);

        if (!fLambda)
        {
            m_currentNodeNonLambdaFunc = pnodeFnc;
        }
    }
    else // if !buildAST
    {
        AnalysisAssert(pnodeFnc);

        if (!fLambda)
        {
            m_currentNodeNonLambdaDeferredFunc = pnodeFnc;
        }

        m_currentNodeDeferredFunc = pnodeFnc;
    }
}