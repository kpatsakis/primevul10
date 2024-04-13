void Parser::RegisterRegexPattern(UnifiedRegex::RegexPattern *const regexPattern)
{
    Assert(regexPattern);

    // ensure a no-throw add behavior here, to catch out of memory exceptions, using the guest arena allocator
    if (!m_registeredRegexPatterns.PrependNoThrow(m_scriptContext->GetGuestArena(), regexPattern))
    {
        Parser::Error(ERRnoMemory);
    }
}