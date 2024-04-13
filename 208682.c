ParseNodePtr Parser::ParseMemberGetSet(OpCode nop, LPCOLESTR* ppNameHint)
{
    ParseNodePtr pnodeName = nullptr;
    Assert(nop == knopGetMember || nop == knopSetMember);
    AssertMem(ppNameHint);
    IdentPtr pid = nullptr;
    bool isComputedName = false;

    *ppNameHint=nullptr;

    switch(m_token.tk)
    {
    default:
        if (!m_token.IsReservedWord())
        {
            Error(ERRnoMemberIdent);
        }
        // fall through
    case tkID:
        pid = m_token.GetIdentifier(m_phtbl);
        *ppNameHint = pid->Psz();
        if (buildAST)
        {
            pnodeName = CreateStrNodeWithScanner(pid);
        }
        break;
    case tkStrCon:
        if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
        {
            Error(ERRES5NoOctal);
        }
        pid = m_token.GetStr();
        *ppNameHint = pid->Psz();
        if (buildAST)
        {
            pnodeName = CreateStrNodeWithScanner(pid);
        }
        break;

    case tkIntCon:
        if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
        {
            Error(ERRES5NoOctal);
        }

        pid = m_pscan->PidFromLong(m_token.GetLong());
        if (buildAST)
        {
            pnodeName = CreateStrNodeWithScanner(pid);
        }
        break;

    case tkFltCon:
        if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
        {
            Error(ERRES5NoOctal);
        }

        pid = m_pscan->PidFromDbl(m_token.GetDouble());
        if (buildAST)
        {
            pnodeName = CreateStrNodeWithScanner(pid);
        }
        break;

    case tkLBrack:
        // Computed property name: get|set [expr] () {  }
        if (!m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
        {
            Error(ERRnoMemberIdent);
        }
        LPCOLESTR emptyHint = nullptr;
        uint32 offset = 0;
        ParseComputedName<buildAST>(&pnodeName, &emptyHint, ppNameHint, &offset);

        isComputedName = true;
        break;
    }

    MemberType memberType;
    ushort flags = fFncMethod | fFncNoName;
    if (nop == knopGetMember)
    {
        memberType = MemberTypeGetter;
        flags |= fFncNoArg;
    }
    else
    {
        Assert(nop == knopSetMember);
        memberType = MemberTypeSetter;
        flags |= fFncOneArg;
    }

    this->m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperPropertyAllowed;
    ParseNodePtr pnodeFnc = ParseFncDecl<buildAST>(flags, *ppNameHint,
        /*needsPIDOnRCurlyScan*/ false, /*resetParsingSuperRestrictionState*/ false);

    if (buildAST)
    {
        pnodeFnc->sxFnc.SetIsAccessor();
        return CreateBinNode(nop, pnodeName, pnodeFnc);
    }
    else
    {
        return nullptr;
    }
}