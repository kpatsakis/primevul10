LPCOLESTR Parser::ConstructFinalHintNode(IdentPtr pClassName, IdentPtr pMemberName, IdentPtr pGetSet, bool isStatic, uint32* nameLength, uint32* pShortNameOffset, bool isComputedName, LPCOLESTR pMemberNameHint)
{
    if ((pMemberName == nullptr && !isComputedName) ||
        (pMemberNameHint == nullptr && isComputedName) ||
        !CONFIG_FLAG(UseFullName))
    {
        return nullptr;
    }

    LPCOLESTR pFinalName = isComputedName? pMemberNameHint : pMemberName->Psz();
    uint32 fullNameHintLength = 0;
    uint32 shortNameOffset = 0;
    if (!isStatic)
    {
        // Add prototype.
        pFinalName = AppendNameHints(wellKnownPropertyPids.prototype, pFinalName, &fullNameHintLength, &shortNameOffset);
    }

    if (pClassName)
    {
        uint32 classNameOffset = 0;
        pFinalName = AppendNameHints(pClassName, pFinalName, &fullNameHintLength, &classNameOffset);
        shortNameOffset += classNameOffset;
    }

    if (pGetSet)
    {
        if (m_scriptContext->GetConfig()->IsES6FunctionNameEnabled())
        {
            // displays as get/set prototype.funcname
            uint32 getSetOffset = 0;
            pFinalName = AppendNameHints(pGetSet, pFinalName, &fullNameHintLength, &getSetOffset, true);
            shortNameOffset += getSetOffset;
        }
        else
        {
            pFinalName = AppendNameHints(pFinalName, pGetSet, &fullNameHintLength, &shortNameOffset);
        }

    }
    if (fullNameHintLength > *nameLength)
    {
        *nameLength = fullNameHintLength;
    }

    if (shortNameOffset > *pShortNameOffset)
    {
        *pShortNameOffset = shortNameOffset;
    }

    return pFinalName;
}