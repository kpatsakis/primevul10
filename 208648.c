ParseNodePtr Parser::ParseMemberList(LPCOLESTR pNameHint, uint32* pNameHintLength, tokens declarationType)
{
    ParseNodePtr pnodeArg = nullptr;
    ParseNodePtr pnodeName = nullptr;
    ParseNodePtr pnodeList = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;
    LPCOLESTR pFullNameHint = nullptr;       // A calculated full name
    uint32 fullNameHintLength = pNameHintLength ? *pNameHintLength : 0;
    uint32 shortNameOffset = 0;
    bool isProtoDeclared = false;

    // we get declaration tkLCurly - when the possible object pattern found under the expression.
    bool isObjectPattern = (declarationType == tkVAR || declarationType == tkLET || declarationType == tkCONST || declarationType == tkLCurly) && IsES6DestructuringEnabled();

    // Check for an empty list
    if (tkRCurly == m_token.tk)
    {
        return nullptr;
    }

    ArenaAllocator tempAllocator(_u("MemberNames"), m_nodeAllocator.GetPageAllocator(), Parser::OutOfMemory);

    bool hasDeferredInitError = false;

    for (;;)
    {
        bool isComputedName = false;
#if DEBUG
        if((m_grfscr & fscrEnforceJSON) && (tkStrCon != m_token.tk || !(m_pscan->IsDoubleQuoteOnLastTkStrCon())))
        {
            Error(ERRsyntax);
        }
#endif
        bool isAsyncMethod = false;
        charcount_t ichMin = 0;
        size_t iecpMin = 0;
        if (m_token.tk == tkID && m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.async && m_scriptContext->GetConfig()->IsES7AsyncAndAwaitEnabled())
        {
            RestorePoint parsedAsync;
            m_pscan->Capture(&parsedAsync);
            ichMin = m_pscan->IchMinTok();
            iecpMin = m_pscan->IecpMinTok();

            m_pscan->ScanForcingPid();
            if (m_token.tk == tkLParen || m_token.tk == tkColon || m_token.tk == tkRCurly || m_pscan->FHadNewLine())
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
        ushort fncDeclFlags = fFncNoName | fFncMethod;
        if (isGenerator)
        {
            if (isAsyncMethod)
            {
                Error(ERRsyntax);
            }
            m_pscan->ScanForcingPid();
            fncDeclFlags |= fFncGenerator;
        }

        IdentPtr pidHint = nullptr;              // A name scoped to current expression
        Token tkHint = m_token;
        charcount_t idHintIchMin = static_cast<charcount_t>(m_pscan->IecpMinTok());
        charcount_t idHintIchLim = static_cast< charcount_t >(m_pscan->IecpLimTok());
        bool wrapInBrackets = false;
        switch (m_token.tk)
        {
        default:
            if (!m_token.IsReservedWord())
            {
                Error(ERRnoMemberIdent);
            }
            // allow reserved words
            wrapInBrackets = true;
            // fall-through
        case tkID:
            pidHint = m_token.GetIdentifier(m_phtbl);
            if (buildAST)
            {
                pnodeName = CreateStrNodeWithScanner(pidHint);
            }
            break;

        case tkStrCon:
            if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
            {
                Error(ERRES5NoOctal);
            }
            wrapInBrackets = true;
            pidHint = m_token.GetStr();
            if (buildAST)
            {
                pnodeName = CreateStrNodeWithScanner(pidHint);
            }
            break;

        case tkIntCon:
            // Object initializers with numeric labels allowed in JS6
            if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
            {
                Error(ERRES5NoOctal);
            }

            pidHint = m_pscan->PidFromLong(m_token.GetLong());
            if (buildAST)
            {
                pnodeName = CreateStrNodeWithScanner(pidHint);
            }
            break;

        case tkFltCon:
            if (IsStrictMode() && m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
            {
                Error(ERRES5NoOctal);
            }

            pidHint = m_pscan->PidFromDbl(m_token.GetDouble());
            if (buildAST)
            {
                pnodeName = CreateStrNodeWithScanner(pidHint);
            }
            wrapInBrackets = true;
            break;

        case tkLBrack:
            // Computed property name: [expr] : value
            if (!m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
            {
                Error(ERRnoMemberIdent);
            }

            ParseComputedName<buildAST>(&pnodeName, &pNameHint, &pFullNameHint, &fullNameHintLength, &shortNameOffset);

            isComputedName = true;
            break;
        }

        if (pFullNameHint == nullptr)
        {
            if (CONFIG_FLAG(UseFullName))
            {
                pFullNameHint = AppendNameHints(pNameHint, pidHint, &fullNameHintLength, &shortNameOffset, false, wrapInBrackets);
            }
            else
            {
                pFullNameHint = pidHint? pidHint->Psz() : nullptr;
                fullNameHintLength = pidHint ? pidHint->Cch() : 0;
                shortNameOffset = 0;
            }
        }

        RestorePoint atPid;
        m_pscan->Capture(&atPid);

        m_pscan->ScanForcingPid();

        if (isGenerator && m_token.tk != tkLParen)
        {
            Error(ERRnoLparen);
        }

        if (tkColon == m_token.tk)
        {
            // It is a syntax error is the production of the form __proto__ : <> occurs more than once. From B.3.1 in spec.
            // Note that previous scan is important because only after that we can determine we have a variable.
            if (!isComputedName && pidHint == wellKnownPropertyPids.__proto__)
            {
                if (isProtoDeclared)
                {
                    Error(ERRsyntax);
                }
                else
                {
                    isProtoDeclared = true;
                }
            }

            m_pscan->Scan();
            ParseNodePtr pnodeExpr = nullptr;
            if (isObjectPattern)
            {
                if (m_token.tk == tkEllipsis)
                {
                    Error(ERRUnexpectedEllipsis);
                }
                pnodeExpr = ParseDestructuredVarDecl<buildAST>(declarationType, declarationType != tkLCurly, nullptr/* *hasSeenRest*/, false /*topLevel*/, false /*allowEmptyExpression*/);

                if (m_token.tk != tkComma && m_token.tk != tkRCurly)
                {
                    if (m_token.IsOperator())
                    {
                        Error(ERRDestructNoOper);
                    }
                    Error(ERRsyntax);
                }
            }
            else
            {
                pnodeExpr = ParseExpr<buildAST>(koplCma, nullptr, TRUE, FALSE, pFullNameHint, &fullNameHintLength, &shortNameOffset);
            }
#if DEBUG
            if((m_grfscr & fscrEnforceJSON) && !IsJSONValid(pnodeExpr))
            {
                Error(ERRsyntax);
            }
#endif
            if (buildAST)
            {
                pnodeArg = CreateBinNode(isObjectPattern ? knopObjectPatternMember : knopMember, pnodeName, pnodeExpr);
                if (pnodeArg->sxBin.pnode1->nop == knopStr)
                {
                    pnodeArg->sxBin.pnode1->sxPid.pid->PromoteAssignmentState();
                }
            }
        }
        else if (m_token.tk == tkLParen && m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
        {
            if (isObjectPattern)
            {
                Error(ERRInvalidAssignmentTarget);
            }
            // Shorthand syntax: foo() {} -> foo: function() {}

            // Rewind to the PID and parse a function expression.
            m_pscan->SeekTo(atPid);
            this->m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperPropertyAllowed;
            ParseNodePtr pnodeFunc = ParseFncDecl<buildAST>(fncDeclFlags | (isAsyncMethod ? fFncAsync : fFncNoFlgs), pFullNameHint,
                /*needsPIDOnRCurlyScan*/ false, /*resetParsingSuperRestrictionState*/ false);

            if (isAsyncMethod)
            {
                pnodeFunc->sxFnc.cbMin = iecpMin;
                pnodeFunc->ichMin = ichMin;
            }
            if (buildAST)
            {
                pnodeArg = CreateBinNode(knopMember, pnodeName, pnodeFunc);
            }
        }
        else if (nullptr != pidHint) //Its either tkID/tkStrCon/tkFloatCon/tkIntCon
        {
            Assert(pidHint->Psz() != nullptr);

            if ((pidHint == wellKnownPropertyPids.get || pidHint == wellKnownPropertyPids.set) &&
                // get/set are only pseudo keywords when they are identifiers (i.e. not strings)
                tkHint.tk == tkID && NextTokenIsPropertyNameStart())
            {
                if (isObjectPattern)
                {
                    Error(ERRInvalidAssignmentTarget);
                }

                LPCOLESTR pNameGetOrSet = nullptr;
                OpCode op = pidHint == wellKnownPropertyPids.get ? knopGetMember : knopSetMember;

                pnodeArg = ParseMemberGetSet<buildAST>(op, &pNameGetOrSet);

                if (CONFIG_FLAG(UseFullName) && buildAST && pnodeArg->sxBin.pnode2->nop == knopFncDecl)
                {
                    if (m_scriptContext->GetConfig()->IsES6FunctionNameEnabled())
                    {
                        // displays as "get object.funcname" or "set object.funcname"
                        uint32 getOrSetOffset = 0;
                        LPCOLESTR intermediateHint = AppendNameHints(pNameHint, pNameGetOrSet, &fullNameHintLength, &shortNameOffset);
                        pFullNameHint = AppendNameHints(pidHint, intermediateHint, &fullNameHintLength, &getOrSetOffset, true);
                        shortNameOffset += getOrSetOffset;
                    }
                    else
                    {
                        // displays as "object.funcname.get" or "object.funcname.set"
                        LPCOLESTR intermediateHint = AppendNameHints(pNameGetOrSet, pidHint, &fullNameHintLength, &shortNameOffset);
                        pFullNameHint = AppendNameHints(pNameHint, intermediateHint, &fullNameHintLength, &shortNameOffset);
                    }
                }
            }
            else if ((m_token.tk == tkRCurly || m_token.tk == tkComma || m_token.tk == tkAsg) && m_scriptContext->GetConfig()->IsES6ObjectLiteralsEnabled())
            {
                // Shorthand {foo} -> {foo:foo} syntax.
                // {foo = <initializer>} supported only when on object pattern rules are being applied
                if (tkHint.tk != tkID)
                {
                    Assert(tkHint.IsReservedWord()
                        || tkHint.tk == tkIntCon || tkHint.tk == tkFltCon || tkHint.tk == tkStrCon);
                    // All keywords are banned in non-strict mode.
                    // Future reserved words are banned in strict mode.
                    if (IsStrictMode() || !tkHint.IsFutureReservedWord(true))
                    {
                        IdentifierExpectedError(tkHint);
                    }
                }

                if (buildAST)
                {
                    CheckArgumentsUse(pidHint, GetCurrentFunctionNode());
                }

                bool couldBeObjectPattern = !isObjectPattern && m_token.tk == tkAsg;

                if (couldBeObjectPattern)
                {
                    declarationType = tkLCurly;
                    isObjectPattern = true;

                    // This may be an error but we are deferring for favouring destructuring.
                    hasDeferredInitError = true;
                }

                ParseNodePtr pnodeIdent = nullptr;
                if (isObjectPattern)
                {
                    m_pscan->SeekTo(atPid);
                    pnodeIdent = ParseDestructuredVarDecl<buildAST>(declarationType, declarationType != tkLCurly, nullptr/* *hasSeenRest*/, false /*topLevel*/, false /*allowEmptyExpression*/);

                    if (m_token.tk != tkComma && m_token.tk != tkRCurly)
                    {
                        if (m_token.IsOperator())
                        {
                            Error(ERRDestructNoOper);
                        }
                        Error(ERRsyntax);
                    }
                }
                else
                {
                    // Add a reference to the hinted name so we can bind it properly.
                    PidRefStack *ref = PushPidRef(pidHint);

                    if (buildAST)
                    {
                        pnodeIdent = CreateNameNode(pidHint, idHintIchMin, idHintIchLim);
                        pnodeIdent->sxPid.SetSymRef(ref);
                    }
                }

                if (buildAST)
                {
                    pnodeArg = CreateBinNode(isObjectPattern && !couldBeObjectPattern ? knopObjectPatternMember : knopMemberShort, pnodeName, pnodeIdent);
                }
            }
            else
            {
                Error(ERRnoColon);
            }
        }
        else
        {
            Error(ERRnoColon);
        }

        if (buildAST)
        {
            Assert(pnodeArg->sxBin.pnode2 != nullptr);
            if (pnodeArg->sxBin.pnode2->nop == knopFncDecl)
            {
                Assert(fullNameHintLength >= shortNameOffset);
                pnodeArg->sxBin.pnode2->sxFnc.hint = pFullNameHint;
                pnodeArg->sxBin.pnode2->sxFnc.hintLength =  fullNameHintLength;
                pnodeArg->sxBin.pnode2->sxFnc.hintOffset  = shortNameOffset;
            }
            AddToNodeListEscapedUse(&pnodeList, &lastNodeRef, pnodeArg);
        }
        pidHint = nullptr;
        pFullNameHint = nullptr;
        if (tkComma != m_token.tk)
        {
            break;
        }
        m_pscan->ScanForcingPid();
        if (tkRCurly == m_token.tk)
        {
            break;
        }
    }

    m_hasDeferredShorthandInitError = m_hasDeferredShorthandInitError || hasDeferredInitError;

    if (buildAST)
    {
        AssertMem(lastNodeRef);
        AssertNodeMem(*lastNodeRef);
        pnodeList->ichLim = (*lastNodeRef)->ichLim;
    }

    return pnodeList;
}