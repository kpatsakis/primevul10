void Parser::FinishScopeInfo(Js::ParseableFunctionInfo *functionBody)
{
    if (!functionBody)
    {
        return;
    }

    Js::ScopeInfo* scopeInfo = functionBody->GetScopeInfo();
    if (!scopeInfo)
    {
        return;
    }

    if (this->IsBackgroundParser())
    {
        PROBE_STACK_NO_DISPOSE(m_scriptContext, Js::Constants::MinStackByteCodeVisitor);
    }
    else
    {
        PROBE_STACK(m_scriptContext, Js::Constants::MinStackByteCodeVisitor);
    }

    int scopeId = scopeInfo->GetScopeId();

    scopeInfo->GetScope()->ForEachSymbol([this, scopeId](Symbol *sym)
    {
        this->BindPidRefsInScope(sym->GetPid(), sym, scopeId);
    });
    PopScope(scopeInfo->GetScope());
    PopStmt(&m_currentBlockInfo->pstmt);
    PopBlockInfo();

    Js::ScopeInfo *paramScopeInfo = scopeInfo->GetParamScopeInfo();
    if (paramScopeInfo)
    {
        scopeId = paramScopeInfo->GetScopeId();
        paramScopeInfo->GetScope()->ForEachSymbol([this, scopeId](Symbol *sym)
        {
            this->BindPidRefsInScope(sym->GetPid(), sym, scopeId);
        });
        PopScope(paramScopeInfo->GetScope());
        PopStmt(&m_currentBlockInfo->pstmt);
        PopBlockInfo();
    }

    Js::ScopeInfo *funcExprScopeInfo = scopeInfo->GetFuncExprScopeInfo();
    if (funcExprScopeInfo)
    {
        scopeId = funcExprScopeInfo->GetScopeId();
        funcExprScopeInfo->GetScope()->ForEachSymbol([this, scopeId](Symbol *sym)
        {
            this->BindPidRefsInScope(sym->GetPid(), sym, scopeId);
        });
        PopScope(funcExprScopeInfo->GetScope());
        PopStmt(&m_currentBlockInfo->pstmt);
        PopBlockInfo();
    }

    FinishScopeInfo(scopeInfo->GetParent());
}