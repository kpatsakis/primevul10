HRESULT Parser::ParseCesu8Source(__out ParseNodePtr* parseTree, LPCUTF8 pSrc, size_t length, ULONG grfsrc, CompileScriptException *pse,
    Js::LocalFunctionId * nextFunctionId, SourceContextInfo * sourceContextInfo)
{
    m_functionBody = nullptr;
    m_parseType = ParseType_Upfront;
    return ParseSourceInternal( parseTree, pSrc, 0, length, 0, false, grfsrc, pse, nextFunctionId, 0, sourceContextInfo);
}