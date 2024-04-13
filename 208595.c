Parser::Parser(Js::ScriptContext* scriptContext, BOOL strictMode, PageAllocator *alloc, bool isBackground)
#endif
    : m_nodeAllocator(_u("Parser"), alloc ? alloc : scriptContext->GetThreadContext()->GetPageAllocator(), Parser::OutOfMemory),
    // use the GuestArena directly for keeping the RegexPattern* alive during byte code generation
    m_registeredRegexPatterns(scriptContext->GetGuestArena())
{
    AssertMsg(size == sizeof(Parser), "verify conditionals affecting the size of Parser agree");
    Assert(scriptContext != nullptr);
    m_isInBackground = isBackground;
    m_phtbl = nullptr;
    m_pscan = nullptr;
    m_deferringAST = FALSE;
    m_stoppedDeferredParse = FALSE;
    m_hasParallelJob = false;
    m_doingFastScan = false;
    m_scriptContext = scriptContext;
    m_pCurrentAstSize = nullptr;
    m_arrayDepth = 0;
    m_funcInArrayDepth = 0;
    m_parenDepth = 0;
    m_funcInArray = 0;
    m_tryCatchOrFinallyDepth = 0;
    m_UsesArgumentsAtGlobal = false;
    m_currentNodeFunc = nullptr;
    m_currentNodeDeferredFunc = nullptr;
    m_currentNodeNonLambdaFunc = nullptr;
    m_currentNodeNonLambdaDeferredFunc = nullptr;
    m_currentNodeProg = nullptr;
    m_currDeferredStub = nullptr;
    m_prevSiblingDeferredStub = nullptr;
    m_pstmtCur = nullptr;
    m_currentBlockInfo = nullptr;
    m_currentScope = nullptr;
    m_currentDynamicBlock = nullptr;
    m_grfscr = fscrNil;
    m_length = 0;
    m_originalLength = 0;
    m_nextFunctionId = nullptr;
    m_errorCallback = nullptr;
    m_uncertainStructure = FALSE;
    m_reparsingLambdaParams = false;
    m_inFIB = false;
    currBackgroundParseItem = nullptr;
    backgroundParseItems = nullptr;
    fastScannedRegExpNodes = nullptr;

    m_fUseStrictMode = strictMode;
    m_InAsmMode = false;
    m_deferAsmJs = true;
    m_scopeCountNoAst = 0;
    m_fExpectExternalSource = 0;

    m_parseType = ParseType_Upfront;

    m_deferEllipsisError = false;
    m_hasDeferredShorthandInitError = false;
    m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperDisallowed;
}