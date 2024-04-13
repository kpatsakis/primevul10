HRESULT Parser::ParseSourceWithOffset(__out ParseNodePtr* parseTree, LPCUTF8 pSrc, size_t offset, size_t cbLength, charcount_t cchOffset,
        bool isCesu8, ULONG grfscr, CompileScriptException *pse, Js::LocalFunctionId * nextFunctionId, ULONG lineNumber, SourceContextInfo * sourceContextInfo,
        Js::ParseableFunctionInfo* functionInfo)
{
    m_functionBody = functionInfo;
    if (m_functionBody)
    {
        m_currDeferredStub = m_functionBody->GetDeferredStubs();
        m_InAsmMode = grfscr & fscrNoAsmJs ? false : m_functionBody->GetIsAsmjsMode();
    }
    m_deferAsmJs = !m_InAsmMode;
    m_parseType = ParseType_Deferred;
    return ParseSourceInternal( parseTree, pSrc, offset, cbLength, cchOffset, !isCesu8, grfscr, pse, nextFunctionId, lineNumber, sourceContextInfo);
}