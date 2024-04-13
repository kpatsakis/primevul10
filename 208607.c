ParseNodePtr Parser::ParseMetaProperty(tokens metaParentKeyword, charcount_t ichMin, _Out_opt_ BOOL* pfCanAssign)
{
    AssertMsg(metaParentKeyword == tkNEW, "Only supported for tkNEW parent keywords");
    AssertMsg(this->m_token.tk == tkDot, "We must be currently sitting on the dot after the parent keyword");

    m_pscan->Scan();

    if (this->m_token.tk == tkID && this->m_token.GetIdentifier(m_phtbl) == this->GetTargetPid())
    {
        ThrowNewTargetSyntaxErrForGlobalScope();
        if (pfCanAssign)
        {
            *pfCanAssign = FALSE;
        }
        if (buildAST)
        {
            return CreateNodeWithScanner<knopNewTarget>(ichMin);
        }
    }
    else
    {
        Error(ERRsyntax);
    }

    return nullptr;
}