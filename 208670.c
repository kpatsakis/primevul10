ParseNodePtr Parser::ParseClassDecl(BOOL isDeclaration, LPCOLESTR pNameHint, uint32 *pHintLength, uint32 *pShortNameOffset)
{
    bool hasConstructor = false;
    bool hasExtends = false;
    IdentPtr name = nullptr;
    ParseNodePtr pnodeName = nullptr;
    ParseNodePtr pnodeConstructor = nullptr;
    ParseNodePtr pnodeExtends = nullptr;
    ParseNodePtr pnodeMembers = nullptr;
    ParseNodePtr *lastMemberNodeRef = nullptr;
    ParseNodePtr pnodeStaticMembers = nullptr;
    ParseNodePtr *lastStaticMemberNodeRef = nullptr;
    uint32 nameHintLength = pHintLength ? *pHintLength : 0;
    uint32 nameHintOffset = pShortNameOffset ? *pShortNameOffset : 0;

    ArenaAllocator tempAllocator(_u("ClassMemberNames"), m_nodeAllocator.GetPageAllocator(), Parser::OutOfMemory);

    ParseNodePtr pnodeClass = nullptr;
    if (buildAST)
    {
        pnodeClass = CreateNode(knopClassDecl);

        CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(Class, m_scriptContext);
    }

    m_pscan->Scan();
    if (m_token.tk == tkID)
    {
        name = m_token.GetIdentifier(m_phtbl);
        m_pscan->Scan();
    }
    else if (isDeclaration)
    {
        IdentifierExpectedError(m_token);
    }

    if (isDeclaration && name == wellKnownPropertyPids.arguments && GetCurrentBlockInfo()->pnodeBlock->sxBlock.blockType == Function)
    {
        GetCurrentFunctionNode()->grfpn |= PNodeFlags::fpnArguments_overriddenByDecl;
    }

    BOOL strictSave = m_fUseStrictMode;
    m_fUseStrictMode = TRUE;

    ParseNodePtr pnodeDeclName = nullptr;
    if (isDeclaration)
    {
        pnodeDeclName = CreateBlockScopedDeclNode(name, knopLetDecl);
    }

    ParseNodePtr *ppnodeScopeSave = nullptr;
    ParseNodePtr *ppnodeExprScopeSave = nullptr;

    ParseNodePtr pnodeBlock = StartParseBlock<buildAST>(PnodeBlockType::Regular, ScopeType_Block);
    if (buildAST)
    {
        PushFuncBlockScope(pnodeBlock, &ppnodeScopeSave, &ppnodeExprScopeSave);
        pnodeClass->sxClass.pnodeBlock = pnodeBlock;
    }

    if (name)
    {
        pnodeName = CreateBlockScopedDeclNode(name, knopConstDecl);
    }

    if (m_token.tk == tkEXTENDS)
    {
        m_pscan->Scan();
        pnodeExtends = ParseExpr<buildAST>();
        hasExtends = true;
    }

    if (m_token.tk != tkLCurly)
    {
        Error(ERRnoLcurly);
    }

    OUTPUT_TRACE_DEBUGONLY(Js::ES6VerboseFlag, _u("Parsing class (%s) : %s\n"), GetParseType(), name ? name->Psz() : _u("anonymous class"));

    RestorePoint beginClass;
    m_pscan->Capture(&beginClass);

    m_pscan->ScanForcingPid();

    IdentPtr pClassNamePid = pnodeName ? pnodeName->sxVar.pid : nullptr;

    for (;;)
    {
        if (m_token.tk == tkSColon)
        {
            m_pscan->ScanForcingPid();
            continue;
        }
        if (m_token.tk == tkRCurly)
        {
            break;
        }

        bool isStatic = m_token.tk == tkSTATIC;
        if (isStatic)
        {
            m_pscan->ScanForcingPid();
        }

        ushort fncDeclFlags = fFncNoName | fFncMethod | fFncClassMember;
        charcount_t ichMin = 0;
        size_t iecpMin = 0;
        ParseNodePtr pnodeMemberName = nullptr;
        IdentPtr pidHint = nullptr;
        IdentPtr memberPid = nullptr;
        LPCOLESTR pMemberNameHint = nullptr;
        uint32     memberNameHintLength = 0;
        uint32     memberNameOffset = 0;
        bool isComputedName = false;
        bool isAsyncMethod = false;

        if (m_token.tk == tkID && m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.async && m_scriptContext->GetConfig()->IsES7AsyncAndAwaitEnabled())
        {
            RestorePoint parsedAsync;
            m_pscan->Capture(&parsedAsync);
            ichMin = m_pscan->IchMinTok();
            iecpMin = m_pscan->IecpMinTok();

            m_pscan->Scan();
            if (m_token.tk == tkLParen || m_pscan->FHadNewLine())
            {
                m_pscan->SeekTo(parsedAsync);
            }
            else
            {
                isAsyncMethod = true;
            }
        }

        bool isGenerator = m_scriptContext->GetConfig()->IsES6GeneratorsEnabled() &&
                           m_token.tk == tkStar;
        if (isGenerator)
        {
            fncDeclFlags |= fFncGenerator;
            m_pscan->ScanForcingPid();
        }


        if (m_token.tk == tkLBrack && m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
        {
            // Computed member name: [expr] () { }
            LPCOLESTR emptyHint = nullptr;
            ParseComputedName<buildAST>(&pnodeMemberName, &emptyHint, &pMemberNameHint, &memberNameHintLength, &memberNameOffset);
            isComputedName = true;
        }
        else // not computed name
        {
            memberPid = this->ParseClassPropertyName(&pidHint);
            if (pidHint)
            {
                pMemberNameHint = pidHint->Psz();
                memberNameHintLength = pidHint->Cch();
            }
        }

        if (buildAST && memberPid)
        {
            pnodeMemberName = CreateStrNodeWithScanner(memberPid);
        }

        if (!isStatic && memberPid == wellKnownPropertyPids.constructor)
        {
            if (hasConstructor || isAsyncMethod)
            {
                Error(ERRsyntax);
            }
            hasConstructor = true;
            LPCOLESTR pConstructorName = nullptr;
            uint32  constructorNameLength = 0;
            uint32  constructorShortNameHintOffset = 0;
            if (pnodeName && pnodeName->sxVar.pid)
            {
                pConstructorName = pnodeName->sxVar.pid->Psz();
                constructorNameLength = pnodeName->sxVar.pid->Cch();
            }
            else
            {
                pConstructorName = pNameHint;
                constructorNameLength = nameHintLength;
                constructorShortNameHintOffset = nameHintOffset;
            }

            {
                AutoParsingSuperRestrictionStateRestorer restorer(this);
                this->m_parsingSuperRestrictionState = hasExtends ? ParsingSuperRestrictionState_SuperCallAndPropertyAllowed : ParsingSuperRestrictionState_SuperPropertyAllowed;
                pnodeConstructor = ParseFncDecl<buildAST>(fncDeclFlags, pConstructorName, /* needsPIDOnRCurlyScan */ true, /* resetParsingSuperRestrictionState = */false);
            }

            if (pnodeConstructor->sxFnc.IsGenerator())
            {
                Error(ERRConstructorCannotBeGenerator);
            }

            Assert(constructorNameLength >= constructorShortNameHintOffset);
            // The constructor function will get the same name as class.
            pnodeConstructor->sxFnc.hint = pConstructorName;
            pnodeConstructor->sxFnc.hintLength = constructorNameLength;
            pnodeConstructor->sxFnc.hintOffset = constructorShortNameHintOffset;
            pnodeConstructor->sxFnc.pid = pnodeName && pnodeName->sxVar.pid ? pnodeName->sxVar.pid : wellKnownPropertyPids.constructor;
            pnodeConstructor->sxFnc.SetIsClassConstructor(TRUE);
            pnodeConstructor->sxFnc.SetHasNonThisStmt();
            pnodeConstructor->sxFnc.SetIsBaseClassConstructor(pnodeExtends == nullptr);
        }
        else
        {
            ParseNodePtr pnodeMember = nullptr;

            bool isMemberNamedGetOrSet = false;
            RestorePoint beginMethodName;
            m_pscan->Capture(&beginMethodName);
            if (memberPid == wellKnownPropertyPids.get || memberPid == wellKnownPropertyPids.set)
            {
                m_pscan->ScanForcingPid();
            }
            if (m_token.tk == tkLParen)
            {
                m_pscan->SeekTo(beginMethodName);
                isMemberNamedGetOrSet = true;
            }

            if ((memberPid == wellKnownPropertyPids.get || memberPid == wellKnownPropertyPids.set) && !isMemberNamedGetOrSet)
            {
                bool isGetter = (memberPid == wellKnownPropertyPids.get);

                if (m_token.tk == tkLBrack && m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
                {
                    // Computed get/set member name: get|set [expr] () { }
                    LPCOLESTR emptyHint = nullptr;
                    ParseComputedName<buildAST>(&pnodeMemberName, &emptyHint, &pMemberNameHint, &memberNameHintLength, &memberNameOffset);
                    isComputedName = true;
                }
                else // not computed name
                {
                    memberPid = this->ParseClassPropertyName(&pidHint);
                }

                if ((isStatic ? (memberPid == wellKnownPropertyPids.prototype) : (memberPid == wellKnownPropertyPids.constructor)) || isAsyncMethod)
                {
                    Error(ERRsyntax);
                }
                if (buildAST && memberPid && !isComputedName)
                {
                    pnodeMemberName = CreateStrNodeWithScanner(memberPid);
                }

                ParseNodePtr pnodeFnc = nullptr;
                {
                    AutoParsingSuperRestrictionStateRestorer restorer(this);
                    this->m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperPropertyAllowed;
                    pnodeFnc = ParseFncDecl<buildAST>(fncDeclFlags | (isGetter ? fFncNoArg : fFncOneArg),
                        pidHint ? pidHint->Psz() : nullptr, /* needsPIDOnRCurlyScan */ true,
                        /* resetParsingSuperRestrictionState */false);
                }

                pnodeFnc->sxFnc.SetIsStaticMember(isStatic);

                if (buildAST)
                {
                    pnodeFnc->sxFnc.SetIsAccessor();
                    pnodeMember = CreateBinNode(isGetter ? knopGetMember : knopSetMember, pnodeMemberName, pnodeFnc);
                    pMemberNameHint = ConstructFinalHintNode(pClassNamePid, pidHint,
                        isGetter ? wellKnownPropertyPids.get : wellKnownPropertyPids.set, isStatic,
                        &memberNameHintLength, &memberNameOffset, isComputedName, pMemberNameHint);
                }
            }
            else
            {
                if (isStatic && (memberPid == wellKnownPropertyPids.prototype))
                {
                    Error(ERRsyntax);
                }

                ParseNodePtr pnodeFnc = nullptr;
                {
                    AutoParsingSuperRestrictionStateRestorer restorer(this);
                    this->m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperPropertyAllowed;

                    if (isAsyncMethod)
                    {
                        fncDeclFlags |= fFncAsync;
                    }
                    pnodeFnc = ParseFncDecl<buildAST>(fncDeclFlags, pidHint ? pidHint->Psz() : nullptr, /* needsPIDOnRCurlyScan */ true, /* resetParsingSuperRestrictionState */false);
                    if (isAsyncMethod)
                    {
                        pnodeFnc->sxFnc.cbMin = iecpMin;
                        pnodeFnc->ichMin = ichMin;
                    }
                }
                pnodeFnc->sxFnc.SetIsStaticMember(isStatic);

                if (buildAST)
                {
                    pnodeMember = CreateBinNode(knopMember, pnodeMemberName, pnodeFnc);
                    pMemberNameHint = ConstructFinalHintNode(pClassNamePid, pidHint, nullptr /*pgetset*/, isStatic, &memberNameHintLength, &memberNameOffset, isComputedName, pMemberNameHint);
                }
            }

            if (buildAST)
            {
                Assert(memberNameHintLength >= memberNameOffset);
                pnodeMember->sxBin.pnode2->sxFnc.hint = pMemberNameHint; // Fully qualified name
                pnodeMember->sxBin.pnode2->sxFnc.hintLength = memberNameHintLength;
                pnodeMember->sxBin.pnode2->sxFnc.hintOffset = memberNameOffset;
                pnodeMember->sxBin.pnode2->sxFnc.pid = memberPid; // Short name

                AddToNodeList(isStatic ? &pnodeStaticMembers : &pnodeMembers, isStatic ? &lastStaticMemberNodeRef : &lastMemberNodeRef, pnodeMember);
            }
        }
    }

    if (buildAST)
    {
        pnodeClass->ichLim = m_pscan->IchLimTok();
    }

    if (!hasConstructor)
    {
        OUTPUT_TRACE_DEBUGONLY(Js::ES6VerboseFlag, _u("Generating constructor (%s) : %s\n"), GetParseType(), name ? name->Psz() : _u("anonymous class"));

        RestorePoint endClass;
        m_pscan->Capture(&endClass);
        m_pscan->SeekTo(beginClass);

        pnodeConstructor = GenerateEmptyConstructor<buildAST>(pnodeExtends != nullptr);
        if (buildAST)
        {
            if (pClassNamePid)
            {
                pnodeConstructor->sxFnc.hint = pClassNamePid->Psz();
                pnodeConstructor->sxFnc.hintLength = pClassNamePid->Cch();
                pnodeConstructor->sxFnc.hintOffset = 0;
            }
            else
            {
                Assert(nameHintLength >= nameHintOffset);
                pnodeConstructor->sxFnc.hint = pNameHint;
                pnodeConstructor->sxFnc.hintLength = nameHintLength;
                pnodeConstructor->sxFnc.hintOffset = nameHintOffset;
            }
            pnodeConstructor->sxFnc.pid = pClassNamePid;
        }

        m_pscan->SeekTo(endClass);
    }

    if (buildAST)
    {
        pnodeConstructor->sxFnc.cbMin = pnodeClass->ichMin;
        pnodeConstructor->sxFnc.cbLim = pnodeClass->ichLim;
        pnodeConstructor->ichMin = pnodeClass->ichMin;
        pnodeConstructor->ichLim = pnodeClass->ichLim;

        PopFuncBlockScope(ppnodeScopeSave, ppnodeExprScopeSave);

        pnodeClass->sxClass.pnodeDeclName = pnodeDeclName;
        pnodeClass->sxClass.pnodeName = pnodeName;
        pnodeClass->sxClass.pnodeConstructor = pnodeConstructor;
        pnodeClass->sxClass.pnodeExtends = pnodeExtends;
        pnodeClass->sxClass.pnodeMembers = pnodeMembers;
        pnodeClass->sxClass.pnodeStaticMembers = pnodeStaticMembers;
        pnodeClass->sxClass.isDefaultModuleExport = false;
    }
    FinishParseBlock(pnodeBlock);

    m_fUseStrictMode = strictSave;

    m_pscan->Scan();

    return pnodeClass;
}