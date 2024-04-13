void Parser::RestoreScopeInfo(Js::ParseableFunctionInfo* functionBody)
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

    RestoreScopeInfo(scopeInfo->GetParent()); // Recursively restore outer func scope info

    Js::ScopeInfo* funcExprScopeInfo = scopeInfo->GetFuncExprScopeInfo();
    if (funcExprScopeInfo)
    {
        funcExprScopeInfo->SetScopeId(m_nextBlockId);
        ParseNodePtr pnodeFncExprScope = StartParseBlockWithCapacity<true>(PnodeBlockType::Function, ScopeType_FuncExpr, funcExprScopeInfo->GetSymbolCount());
        Scope *scope = pnodeFncExprScope->sxBlock.scope;
        funcExprScopeInfo->GetScopeInfo(this, nullptr, nullptr, scope);
    }

    Js::ScopeInfo* paramScopeInfo = scopeInfo->GetParamScopeInfo();
    if (paramScopeInfo)
    {
        paramScopeInfo->SetScopeId(m_nextBlockId);
        ParseNodePtr pnodeFncExprScope = StartParseBlockWithCapacity<true>(PnodeBlockType::Parameter, ScopeType_Parameter, paramScopeInfo->GetSymbolCount());
        Scope *scope = pnodeFncExprScope->sxBlock.scope;
        paramScopeInfo->GetScopeInfo(this, nullptr, nullptr, scope);
    }

    scopeInfo->SetScopeId(m_nextBlockId);
    ParseNodePtr pnodeFncScope = nullptr;
    if (scopeInfo->IsGlobalEval())
    {
        pnodeFncScope = StartParseBlockWithCapacity<true>(PnodeBlockType::Regular, ScopeType_GlobalEvalBlock, scopeInfo->GetSymbolCount());
    }
    else
    {
        pnodeFncScope = StartParseBlockWithCapacity<true>(PnodeBlockType::Function, ScopeType_FunctionBody, scopeInfo->GetSymbolCount());
    }
    Scope *scope = pnodeFncScope->sxBlock.scope;
    scopeInfo->GetScopeInfo(this, nullptr, nullptr, scope);
}