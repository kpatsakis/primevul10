bool Parser::CheckAsmjsModeStrPid(IdentPtr pid)
{
#ifdef ASMJS_PLAT
    if (!CONFIG_FLAG_RELEASE(Asmjs))
    {
        return false;
    }

    bool isAsmCandidate = (pid != nullptr &&
        AutoSystemInfo::Data.SSE2Available() &&
        pid->Cch() == 7 &&
        !m_pscan->IsEscapeOnLastTkStrCon() &&
        wcsncmp(pid->Psz(), _u("use asm"), 10) == 0);

    if (isAsmCandidate && m_scriptContext->IsScriptContextInDebugMode())
    {
        // We would like to report this to debugger - they may choose to disable debugging.
        // TODO : localization of the string?
        m_scriptContext->RaiseMessageToDebugger(DEIT_ASMJS_IN_DEBUGGING, _u("AsmJs initialization error - AsmJs disabled due to script debugger"), !m_sourceContextInfo->IsDynamic() ? m_sourceContextInfo->url : nullptr);
        return false;
    }

    return isAsmCandidate && !(m_grfscr & fscrNoAsmJs);
#else
    return false;
#endif
}