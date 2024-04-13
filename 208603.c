ULONG Parser::GetDeferralThreshold(bool isProfileLoaded)
{
#ifdef ENABLE_DEBUG_CONFIG_OPTIONS
    if (CONFIG_FLAG(ForceDeferParse) ||
        PHASE_FORCE1(Js::DeferParsePhase) ||
        Js::Configuration::Global.flags.IsEnabled(Js::ForceUndoDeferFlag))
    {
        return 0;
    }
    else if (Js::Configuration::Global.flags.IsEnabled(Js::DeferParseFlag))
    {
        return Js::Configuration::Global.flags.DeferParse;
    }
    else
#endif
    {
        if (isProfileLoaded)
        {
            return DEFAULT_CONFIG_ProfileBasedDeferParseThreshold;
        }
        return DEFAULT_CONFIG_DeferParseThreshold;
    }
}