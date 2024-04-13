ParseNodePtr Parser::CreateAsyncSpawnGenerator()
{
    ParseNodePtr pnodeFncGenerator = nullptr;

    pnodeFncGenerator = CreateDummyFuncNode(false);
    pnodeFncGenerator->sxFnc.functionId = (*m_nextFunctionId)++;

    pnodeFncGenerator->sxFnc.cbMin = m_pscan->IecpMinTok();
    pnodeFncGenerator->sxFnc.cbLim = m_pscan->IecpLimTok();
    pnodeFncGenerator->sxFnc.lineNumber = m_pscan->LineCur();
    pnodeFncGenerator->sxFnc.columnNumber = CalculateFunctionColumnNumber();
    pnodeFncGenerator->sxFnc.SetNested(m_currentNodeFunc != nullptr);
    pnodeFncGenerator->sxFnc.SetStrictMode(IsStrictMode());

    pnodeFncGenerator->sxFnc.SetIsGenerator();
    pnodeFncGenerator->sxFnc.SetIsLambda();
    pnodeFncGenerator->sxFnc.scope = nullptr;

    AppendFunctionToScopeList(false, pnodeFncGenerator);

    return pnodeFncGenerator;
}