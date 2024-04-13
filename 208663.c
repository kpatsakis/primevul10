LPCOLESTR Parser::AppendNameHints(IdentPtr left, LPCOLESTR right, uint32 *pNameLength, uint32 *pShortNameOffset, bool ignoreAddDotWithSpace, bool wrapInBrackets)
{
    uint32 rightLen = (right == nullptr) ? 0 : (uint32) wcslen(right);

    if (pShortNameOffset != nullptr)
    {
        *pShortNameOffset = 0;
    }

    Assert(rightLen <= ULONG_MAX); // name hints should not exceed ULONG_MAX characters

    if (left == nullptr && !wrapInBrackets)
    {
        *pNameLength = rightLen;
        return right;
    }

    LPCOLESTR leftStr = _u("");
    uint32 leftLen = 0;

    if (left != nullptr) // if wrapInBrackets is true
    {
        leftStr = left->Psz();
        leftLen = left->Cch();
    }

    if (rightLen == 0 && !wrapInBrackets)
    {
        *pNameLength = leftLen;
        return left->Psz();
    }

    return AppendNameHints(leftStr, leftLen, right, rightLen, pNameLength, pShortNameOffset, ignoreAddDotWithSpace, wrapInBrackets);
}