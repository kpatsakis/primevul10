LPCOLESTR Parser::AppendNameHints(LPCOLESTR left, IdentPtr right, uint32 *pNameLength, uint32 *pShortNameOffset, bool ignoreAddDotWithSpace, bool wrapInBrackets)
{
    uint32 leftLen = (left == nullptr) ? 0 : (uint32) wcslen(left);

    if (pShortNameOffset != nullptr)
    {
        *pShortNameOffset = 0;
    }

    Assert(leftLen <= ULONG_MAX); // name hints should not exceed ULONG_MAX characters

    if (left == nullptr || (leftLen == 0 && !wrapInBrackets))
    {
        if (right != nullptr)
        {
            *pNameLength = right->Cch();
            return right->Psz();
        }
        return nullptr;
    }

    if (right == nullptr)
    {
        *pNameLength = leftLen;
        return left;
    }
    uint32 rightLen = right->Cch();

    return AppendNameHints(left, leftLen, right->Psz(), rightLen, pNameLength, pShortNameOffset, ignoreAddDotWithSpace, wrapInBrackets);
}