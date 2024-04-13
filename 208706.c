bool Parser::ParseFncNames(ParseNodePtr pnodeFnc, ParseNodePtr pnodeFncParent, ushort flags, ParseNodePtr **pLastNodeRef)
{
    BOOL fDeclaration = flags & fFncDeclaration;
    BOOL fIsAsync = flags & fFncAsync;
    ParseNodePtr pnodeT;
    charcount_t ichMinNames, ichLimNames;

    // Get the names to bind to.
    /*
    * KaushiS [5/15/08]:
    * ECMAScript defines a FunctionExpression as follows:
    *
    * "function" [Identifier] ( [FormalParameterList] ) { FunctionBody }
    *
    * The function name being optional is omitted by most real world
    * code that uses a FunctionExpression to define a function. This however
    * is problematic for tools because there isn't a function name that
    * the runtime can provide.
    *
    * To fix this (primarily for the profiler), I'm adding simple, static
    * name inferencing logic to the parser. When it encounters the following
    * productions
    *
    *   "var" Identifier "=" FunctionExpression
    *   "var" IdentifierA.IdentifierB...Identifier "=" FunctionExpression
    *   Identifier = FunctionExpression
    *   "{" Identifier: FunctionExpression "}"
    *
    * it associates Identifier with the function created by the
    * FunctionExpression. This identifier is *not* the function's name. It
    * is ignored by the runtime and is only an additional piece of information
    * about the function (function name hint) that tools could opt to
    * surface.
    */

    m_pscan->Scan();

    // If generators are enabled then we are in a recent enough version
    // that deferred parsing will create a parse node for pnodeFnc and
    // it is safe to assume it is not null.
    if (flags & fFncGenerator)
    {
        Assert(m_scriptContext->GetConfig()->IsES6GeneratorsEnabled());
        pnodeFnc->sxFnc.SetIsGenerator();
    }
    else if (m_scriptContext->GetConfig()->IsES6GeneratorsEnabled() &&
        m_token.tk == tkStar &&
        !(flags & fFncClassMember))
    {
        if (!fDeclaration)
        {
            bool fPreviousYieldIsKeyword = m_pscan->SetYieldIsKeyword(!fDeclaration);
            m_pscan->Scan();
            m_pscan->SetYieldIsKeyword(fPreviousYieldIsKeyword);
        }
        else
        {
            m_pscan->Scan();
        }

        pnodeFnc->sxFnc.SetIsGenerator();
    }

    if (fIsAsync)
    {
        if (pnodeFnc->sxFnc.IsGenerator())
        {
            Error(ERRsyntax);
        }
        pnodeFnc->sxFnc.SetIsAsync();
    }

    if (pnodeFnc)
    {
        pnodeFnc->sxFnc.pnodeName = nullptr;
    }

    if ((m_token.tk != tkID || flags & fFncNoName)
        && (IsStrictMode() || (pnodeFnc && pnodeFnc->sxFnc.IsGenerator()) || m_token.tk != tkYIELD || fDeclaration)) // Function expressions can have the name yield even inside generator functions
    {
        if (fDeclaration  ||
            m_token.IsReservedWord())  // For example:  var x = (function break(){});
        {
            IdentifierExpectedError(m_token);
        }
        return false;
    }

    ichMinNames = m_pscan->IchMinTok();


    Assert(m_token.tk == tkID || (m_token.tk == tkYIELD && !fDeclaration));

    if (IsStrictMode())
    {
        CheckStrictModeEvalArgumentsUsage(m_token.GetIdentifier(m_phtbl));
    }
    Token tokenBase = m_token;
    charcount_t ichMinBase = m_pscan->IchMinTok();
    charcount_t ichLimBase = m_pscan->IchLimTok();

    m_pscan->Scan();

    IdentPtr pidBase = tokenBase.GetIdentifier(m_phtbl);
    pnodeT = CreateDeclNode(knopVarDecl, pidBase, STFunction);
    pnodeT->ichMin = ichMinBase;
    pnodeT->ichLim = ichLimBase;

    if (fDeclaration &&
        pnodeFncParent &&
        pnodeFncParent->sxFnc.pnodeName &&
        pnodeFncParent->sxFnc.pnodeName->nop == knopVarDecl &&
        pnodeFncParent->sxFnc.pnodeName->sxVar.pid == pidBase)
    {
        pnodeFncParent->sxFnc.SetNameIsHidden();
    }

    if (buildAST)
    {
        AnalysisAssert(pnodeFnc);
        ichLimNames = pnodeT->ichLim;
        AddToNodeList(&pnodeFnc->sxFnc.pnodeName, pLastNodeRef, pnodeT);

        pnodeFnc->sxFnc.pnodeName->ichMin = ichMinNames;
        pnodeFnc->sxFnc.pnodeName->ichLim = ichLimNames;
        if (knopVarDecl == pnodeFnc->sxFnc.pnodeName->nop)
        {
            // Only one name (the common case).
            pnodeFnc->sxFnc.pid = pnodeFnc->sxFnc.pnodeName->sxVar.pid;
        }
        else
        {
            // Multiple names. Turn the source into an IdentPtr.
            pnodeFnc->sxFnc.pid = m_phtbl->PidHashNameLen(
                m_pscan->PchBase() + ichMinNames, 
                m_pscan->AdjustedLast(),
                ichLimNames - ichMinNames);
        }
    }

    return true;
}