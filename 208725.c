LPCOLESTR Parser::GetFunctionName(ParseNodePtr pnodeFnc, LPCOLESTR pNameHint)
{
    LPCOLESTR name = nullptr;
    if(pnodeFnc->sxFnc.pnodeName != nullptr && knopVarDecl == pnodeFnc->sxFnc.pnodeName->nop)
    {
        name = pnodeFnc->sxFnc.pnodeName->sxVar.pid->Psz();
    }
    if(name == nullptr && pNameHint != nullptr)
    {
        name = pNameHint;
    }
    if(name == nullptr && m_functionBody != nullptr)
    {
        name = m_functionBody->GetExternalDisplayName();
    }
    else if(name == nullptr)
    {
        name = Js::Constants::AnonymousFunction;
    }
    return name;
}