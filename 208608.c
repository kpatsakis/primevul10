LPCOLESTR Parser::AppendNameHints(IdentPtr left, IdentPtr right, uint32 *pNameLength, uint32 *pShortNameOffset, bool ignoreAddDotWithSpace, bool wrapInBrackets)
{
    if (pShortNameOffset != nullptr)
    {
        *pShortNameOffset = 0;
    }

    if (left == nullptr && !wrapInBrackets)
    {
        if (right)
        {
            *pNameLength = right->Cch();
            return right->Psz();
        }
        return nullptr;
    }

    uint32 leftLen = 0;
    LPCOLESTR leftStr = _u("");

    if (left != nullptr) // if wrapInBrackets is true
    {
        leftStr = left->Psz();
        leftLen = left->Cch();
    }

    if (right == nullptr)
    {
        *pNameLength = leftLen;
        return left->Psz();
    }
    uint32 rightLen = right->Cch();

    return AppendNameHints(leftStr, leftLen, right->Psz(), rightLen, pNameLength, pShortNameOffset, ignoreAddDotWithSpace, wrapInBrackets);
}