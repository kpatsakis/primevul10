bool Parser::FncDeclAllowedWithoutContext(ushort flags)
{
    // Statement context required for strict mode, async functions, and generators.
    // Note that generators aren't detected yet when this method is called; they're checked elsewhere.
    return !IsStrictMode() && !(flags & fFncAsync);
}