ParseNodePtr Parser::ParseDefaultExportClause()
{
    Assert(m_token.tk == tkDEFAULT);

    m_pscan->Scan();
    ParseNodePtr pnode = nullptr;
    ushort flags = fFncNoFlgs;

    switch (m_token.tk)
    {
    case tkCLASS:
        {
            if (!m_scriptContext->GetConfig()->IsES6ClassAndExtendsEnabled())
            {
                goto LDefault;
            }

            // Before we parse the class itself we need to know if the class has an identifier name.
            // If it does, we'll treat this class as an ordinary class declaration which will bind
            // it to that name. Otherwise the class should parse as a nameless class expression and
            // bind only to the export binding.
            BOOL classHasName = false;
            RestorePoint parsedClass;
            m_pscan->Capture(&parsedClass);
            m_pscan->Scan();

            if (m_token.tk == tkID)
            {
                classHasName = true;
            }

            m_pscan->SeekTo(parsedClass);
            pnode = ParseClassDecl<buildAST>(classHasName, nullptr, nullptr, nullptr);

            if (buildAST)
            {
                AnalysisAssert(pnode != nullptr);
                Assert(pnode->nop == knopClassDecl);

                pnode->sxClass.SetIsDefaultModuleExport(true);
            }

            break;
        }
    case tkID:
        // If we parsed an async token, it could either modify the next token (if it is a
        // function token) or it could be an identifier (let async = 0; export default async;).
        // To handle both cases, when we parse an async token we need to keep the parser state
        // and rewind if the next token is not function.
        if (wellKnownPropertyPids.async == m_token.GetIdentifier(m_phtbl))
        {
            RestorePoint parsedAsync;
            m_pscan->Capture(&parsedAsync);
            m_pscan->Scan();
            if (m_token.tk == tkFUNCTION)
            {
                // Token after async is function, consume the async token and continue to parse the
                // function as an async function.
                flags |= fFncAsync;
                goto LFunction;
            }
            // Token after async is not function, no idea what the async token is supposed to mean
            // so rewind and let the default case handle it.
            m_pscan->SeekTo(parsedAsync);
        }
        goto LDefault;
        break;
    case tkFUNCTION:
        {
LFunction:
            // We just parsed a function token but we need to figure out if the function
            // has an identifier name or not before we call the helper.
            RestorePoint parsedFunction;
            m_pscan->Capture(&parsedFunction);
            m_pscan->Scan();

            if (m_token.tk == tkStar)
            {
                // If we saw 'function*' that indicates we are going to parse a generator,
                // but doesn't tell us if the generator has an identifier or not.
                // Skip the '*' token for now as it doesn't matter yet.
                m_pscan->Scan();
            }

            // We say that if the function has an identifier name, it is a 'normal' declaration
            // and should create a binding to that identifier as well as one for our default export.
            if (m_token.tk == tkID)
            {
                flags |= fFncDeclaration;
            }
            else
            {
                flags |= fFncNoName;
            }

            // Rewind back to the function token and let the helper handle the parsing.
            m_pscan->SeekTo(parsedFunction);
            pnode = ParseFncDecl<buildAST>(flags);
            
            if (buildAST)
            {
                AnalysisAssert(pnode != nullptr);
                Assert(pnode->nop == knopFncDecl);

                pnode->sxFnc.SetIsDefaultModuleExport(true);
            }
            break;
        }
    default:
LDefault:
        {
            ParseNodePtr pnodeExpression = ParseExpr<buildAST>();

            // Consider: Can we detect this syntax error earlier?
            if (pnodeExpression && pnodeExpression->nop == knopComma)
            {
                Error(ERRsyntax);
            }

            if (buildAST)
            {
                AnalysisAssert(pnodeExpression != nullptr);

                // Mark this node as the default module export. We need to make sure it is put into the correct
                // module export slot when we emit the node.
                pnode = CreateNode(knopExportDefault);
                pnode->sxExportDefault.pnodeExpr = pnodeExpression;
            }
            break;
        }
    }

    IdentPtr exportName = wellKnownPropertyPids._default;
    IdentPtr localName = wellKnownPropertyPids._starDefaultStar;
    AddModuleImportOrExportEntry(EnsureModuleLocalExportEntryList(), nullptr, localName, exportName, nullptr);

    return pnode;
}