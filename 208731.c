LPCOLESTR Parser::AppendNameHints(LPCOLESTR leftStr, uint32 leftLen, LPCOLESTR rightStr, uint32 rightLen, uint32 *pNameLength, uint32 *pShortNameOffset, bool ignoreAddDotWithSpace, bool wrapInBrackets)
{
    Assert(rightStr != nullptr);
    Assert(leftLen  != 0 || wrapInBrackets);
    Assert(rightLen != 0 || wrapInBrackets);

    bool ignoreDot = rightStr[0] == _u('[') && !wrapInBrackets;//if we wrap in brackets it can be a string literal which can have brackets at the first char
    uint32 totalLength = leftLen + rightLen + ((ignoreDot) ? 1 : 2); // 1 (for dot or [) + 1 (for null termination)

    if (wrapInBrackets)
    {
        totalLength++; //1 for ']';
    }
    WCHAR * finalName = AllocateStringOfLength(totalLength);

    if (leftStr != nullptr && leftLen != 0)
    {
        wcscpy_s(finalName, leftLen + 1, leftStr);
    }

    if (ignoreAddDotWithSpace)
    {
        finalName[leftLen++] = (OLECHAR)_u(' ');
    }
    // mutually exclusive from ignoreAddDotWithSpace which is used for getters/setters

    else if (wrapInBrackets)
    {
        finalName[leftLen++] = (OLECHAR)_u('[');
        finalName[totalLength-2] = (OLECHAR)_u(']');
    }
    else if (!ignoreDot)
    {
        finalName[leftLen++] = (OLECHAR)_u('.');
    }
    //ignore case falls through
    js_wmemcpy_s(finalName + leftLen, rightLen, rightStr, rightLen);
    finalName[totalLength-1] = (OLECHAR)_u('\0');

    if (pNameLength != nullptr)
    {
        *pNameLength = totalLength - 1;
    }
    if (pShortNameOffset != nullptr)
    {
        *pShortNameOffset = leftLen;
    }

    return finalName;
}