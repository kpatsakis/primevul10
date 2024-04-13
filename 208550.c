HRESULT Parser::ParseSourceInternal(
    __out ParseNodePtr* parseTree, LPCUTF8 pszSrc, size_t offsetInBytes, size_t encodedCharCount, charcount_t offsetInChars,
    bool fromExternal, ULONG grfscr, CompileScriptException *pse, Js::LocalFunctionId * nextFunctionId, ULONG lineNumber, SourceContextInfo * sourceContextInfo)
{
    AssertMem(parseTree);
    AssertPsz(pszSrc);
    AssertMemN(pse);
   
    if (this->IsBackgroundParser())
    {
        PROBE_STACK_NO_DISPOSE(m_scriptContext, Js::Constants::MinStackDefault);
    }
    else
    {
        PROBE_STACK(m_scriptContext, Js::Constants::MinStackDefault);
    }

#ifdef PROFILE_EXEC
    m_scriptContext->ProfileBegin(Js::ParsePhase);
#endif
    JS_ETW_INTERNAL(EventWriteJSCRIPT_PARSE_START(m_scriptContext,0));

    *parseTree = NULL;
    m_sourceLim = 0;

    m_grfscr = grfscr;
    m_sourceContextInfo = sourceContextInfo;

    ParseNodePtr pnodeBase = NULL;
    HRESULT hr;
    SmartFPUControl smartFpuControl;

    DebugOnly( m_err.fInited = TRUE; )

    try
    {
        this->PrepareScanner(fromExternal);

        if ((grfscr & fscrEvalCode) != 0)
        {
            this->m_parsingSuperRestrictionState = Parser::ParsingSuperRestrictionState_SuperPropertyAllowed;
        }

        if ((grfscr & fscrIsModuleCode) != 0)
        {
            // Module source flag should not be enabled unless module is enabled
            Assert(m_scriptContext->GetConfig()->IsES6ModuleEnabled());

            // Module code is always strict mode code.
            this->m_fUseStrictMode = TRUE;
        }

        // parse the source
        pnodeBase = Parse(pszSrc, offsetInBytes, encodedCharCount, offsetInChars, grfscr, lineNumber, nextFunctionId, pse);

        AssertNodeMem(pnodeBase);

        // Record the actual number of words parsed.
        m_sourceLim = pnodeBase->ichLim - offsetInChars;

        // TODO: The assert can be false positive in some scenarios and chuckj to fix it later
        // Assert(utf8::ByteIndexIntoCharacterIndex(pszSrc + offsetInBytes, encodedCharCount, fromExternal ? utf8::doDefault : utf8::doAllowThreeByteSurrogates) == m_sourceLim);

#if DBG_DUMP
        if (Js::Configuration::Global.flags.Trace.IsEnabled(Js::ParsePhase))
        {
            PrintPnodeWIndent(pnodeBase,4);
            fflush(stdout);
        }
#endif

        *parseTree = pnodeBase;

        hr = NOERROR;
    }
    catch(ParseExceptionObject& e)
    {
        m_err.m_hr = e.GetError();
        hr = pse->ProcessError( m_pscan, m_err.m_hr, pnodeBase);
    }

    if (this->m_hasParallelJob)
    {
#if ENABLE_BACKGROUND_PARSING
        ///// Wait here for remaining jobs to finish. Then look for errors, do final const bindings.
        // pleath TODO: If there are remaining jobs, let the main thread help finish them.
        BackgroundParser *bgp = m_scriptContext->GetBackgroundParser();
        Assert(bgp);

        CompileScriptException se;
        this->WaitForBackgroundJobs(bgp, &se);

        BackgroundParseItem *failedItem = bgp->GetFailedBackgroundParseItem();
        if (failedItem)
        {
            CompileScriptException *bgPse = failedItem->GetPSE();
            Assert(bgPse);
            *pse = *bgPse;
            hr = failedItem->GetHR();
            bgp->SetFailedBackgroundParseItem(nullptr);
        }

        if (this->fastScannedRegExpNodes != nullptr)
        {
            this->FinishBackgroundRegExpNodes();
        }

        for (BackgroundParseItem *item = this->backgroundParseItems; item; item = item->GetNext())
        {
            Parser *parser = item->GetParser();
            parser->FinishBackgroundPidRefs(item, this != parser);
        }
#endif
    }

    // done with the scanner
    RELEASEPTR(m_pscan);

#ifdef PROFILE_EXEC
    m_scriptContext->ProfileEnd(Js::ParsePhase);
#endif
    JS_ETW_INTERNAL(EventWriteJSCRIPT_PARSE_STOP(m_scriptContext, 0));
    
    return hr;
}