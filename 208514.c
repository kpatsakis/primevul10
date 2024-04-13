ParseNodePtr Parser::ParseSuper(ParseNodePtr pnode, bool fAllowCall)
{
    ParseNodePtr currentNodeFunc = GetCurrentFunctionNode();

    if (buildAST) {
        pnode = CreateNodeWithScanner<knopSuper>();
    }

    m_pscan->ScanForcingPid();

    switch (m_token.tk)
    {
    case tkDot:     // super.prop
    case tkLBrack:  // super[foo]
    case tkLParen:  // super(args)
        break;

    default:
        Error(ERRInvalidSuper);
        break;
    }

    if (!fAllowCall && (m_token.tk == tkLParen))
    {
        Error(ERRInvalidSuper); // new super() is not allowed
    }
    else if (this->m_parsingSuperRestrictionState == ParsingSuperRestrictionState_SuperCallAndPropertyAllowed)
    {
        // Any super access is good within a class constructor
    }
    else if (this->m_parsingSuperRestrictionState == ParsingSuperRestrictionState_SuperPropertyAllowed)
    {
        if (m_token.tk == tkLParen)
        {
            if ((this->m_grfscr & fscrEval) == fscrNil)
            {
                // Cannot call super within a class member
                Error(ERRInvalidSuper);
            }
            else
            {
                Js::JavascriptFunction * caller = nullptr;
                if (Js::JavascriptStackWalker::GetCaller(&caller, m_scriptContext))
                {
                    Js::FunctionBody * callerBody = caller->GetFunctionBody();
                    Assert(callerBody);
                    if (!callerBody->GetFunctionInfo()->GetAllowDirectSuper())
                    {
                        Error(ERRInvalidSuper);
                    }
                }
            }
        }
    }
    else
    {
        // Anything else is an error
        Error(ERRInvalidSuper);
    }

    currentNodeFunc->sxFnc.SetHasSuperReference(TRUE);
    CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(Super, m_scriptContext);
    return pnode;
}