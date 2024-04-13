LPCOLESTR Parser::AppendNameHints(LPCOLESTR left, LPCOLESTR right, uint32 *pNameLength, uint32 *pShortNameOffset, bool ignoreAddDotWithSpace, bool wrapInBrackets)
{
    uint32 leftLen = (left == nullptr) ? 0 : (uint32) wcslen(left);
    uint32 rightLen = (right == nullptr) ? 0 : (uint32) wcslen(right);
    if (pShortNameOffset != nullptr)
    {
        *pShortNameOffset = 0;
    }
    Assert(rightLen <= ULONG_MAX && leftLen <= ULONG_MAX); // name hints should not exceed ULONG_MAX characters

    if (leftLen == 0 && !wrapInBrackets)
    {
        *pNameLength = right ? rightLen : 0;
        return right;
    }

    if (rightLen == 0 && !wrapInBrackets)
    {
        *pNameLength = leftLen;
        return left;
    }

    return AppendNameHints(left, leftLen, right, rightLen, pNameLength, pShortNameOffset, ignoreAddDotWithSpace, wrapInBrackets);
}