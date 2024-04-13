bool Parser::CheckStrictModeStrPid(IdentPtr pid)
{
#ifdef ENABLE_DEBUG_CONFIG_OPTIONS
    if (Js::Configuration::Global.flags.NoStrictMode)
        return false;
#endif

    return pid != nullptr &&
        pid->Cch() == 10 &&
        !m_pscan->IsEscapeOnLastTkStrCon() &&
        wcsncmp(pid->Psz(), _u("use strict"), 10) == 0;
}