ParseNodePtr Parser::ParseExportDeclaration()
{
    Assert(m_scriptContext->GetConfig()->IsES6ModuleEnabled());
    Assert(m_token.tk == tkEXPORT);

    if (!IsImportOrExportStatementValidHere())
    {
        Error(ERRInvalidModuleImportOrExport);
    }

    ParseNodePtr pnode = nullptr;
    IdentPtr moduleIdentifier = nullptr;
    tokens declarationType;

    // We just parsed an export token. Next valid tokens are *, {, var, let, const, async, function, class, default.
    m_pscan->Scan();

    switch (m_token.tk)
    {
    case tkStar:
        m_pscan->Scan();

        // A star token in an export declaration must be followed by a from clause which begins with a token 'from'.
        moduleIdentifier = ParseImportOrExportFromClause<buildAST>(true);

        if (buildAST)
        {
            Assert(moduleIdentifier != nullptr);

            AddModuleSpecifier(moduleIdentifier);
            IdentPtr importName = wellKnownPropertyPids._star;

            AddModuleImportOrExportEntry(EnsureModuleStarExportEntryList(), importName, nullptr, nullptr, moduleIdentifier);
        }

        break;

    case tkLCurly:
        {
            ModuleImportOrExportEntryList exportEntryList(&m_nodeAllocator);

            ParseNamedImportOrExportClause<buildAST>(&exportEntryList, true);

            m_pscan->Scan();

            // Export clause may be followed by a from clause.
            moduleIdentifier = ParseImportOrExportFromClause<buildAST>(false);

            if (buildAST)
            {
                if (moduleIdentifier != nullptr)
                {
                    AddModuleSpecifier(moduleIdentifier);
                }

                exportEntryList.Map([this, moduleIdentifier](ModuleImportOrExportEntry& exportEntry) {
                    if (moduleIdentifier != nullptr)
                    {
                        exportEntry.moduleRequest = moduleIdentifier;

                        // We need to swap localname and importname when this is a re-export.
                        exportEntry.importName = exportEntry.localName;
                        exportEntry.localName = nullptr;

                        AddModuleImportOrExportEntry(EnsureModuleIndirectExportEntryList(), &exportEntry);
                    }
                    else
                    {
                        AddModuleImportOrExportEntry(EnsureModuleLocalExportEntryList(), &exportEntry);
                    }
                });

                exportEntryList.Clear();
            }
        }
        break;

    case tkID:
        {
            IdentPtr pid = m_token.GetIdentifier(m_phtbl);

            if (wellKnownPropertyPids.let == pid)
            {
                declarationType = tkLET;
                goto ParseVarDecl;
            }
            if (wellKnownPropertyPids.async == pid && m_scriptContext->GetConfig()->IsES7AsyncAndAwaitEnabled())
            {
                // In module export statements, async token is only valid if it's followed by function.
                // We need to check here because ParseStatement would think 'async = 20' is a var decl.
                RestorePoint parsedAsync;
                m_pscan->Capture(&parsedAsync);
                m_pscan->Scan();
                if (m_token.tk == tkFUNCTION)
                {
                    // Token after async is function, rewind to the async token and let ParseStatement handle it.
                    m_pscan->SeekTo(parsedAsync);
                    goto ParseFunctionDecl;
                }
                // Token after async is not function, it's a syntax error.
            }
            goto ErrorToken;
        }
    case tkVAR:
    case tkLET:
    case tkCONST:
        {
            declarationType = m_token.tk;

ParseVarDecl:
            m_pscan->Scan();

            pnode = ParseVariableDeclaration<buildAST>(declarationType, m_pscan->IchMinTok());

            if (buildAST)
            {
                ParseNodePtr temp = pnode;
                while (temp->nop == knopList)
                {
                    ParseNodePtr varDeclNode = temp->sxBin.pnode1;
                    temp = temp->sxBin.pnode2;

                    AddModuleLocalExportEntry(varDeclNode);
                }
                AddModuleLocalExportEntry(temp);
            }
        }
        break;

    case tkFUNCTION:
    case tkCLASS:
        {
ParseFunctionDecl:
            pnode = ParseStatement<buildAST>();

            if (buildAST)
            {
                IdentPtr localName;
                if (pnode->nop == knopClassDecl)
                {
                    pnode->sxClass.pnodeName->sxVar.sym->SetIsModuleExportStorage(true);
                    pnode->sxClass.pnodeDeclName->sxVar.sym->SetIsModuleExportStorage(true);
                    localName = pnode->sxClass.pnodeName->sxVar.pid;
                }
                else
                {
                    Assert(pnode->nop == knopFncDecl);

                    pnode->sxFnc.GetFuncSymbol()->SetIsModuleExportStorage(true);
                    localName = pnode->sxFnc.pid;
                }
                Assert(localName != nullptr);

                AddModuleImportOrExportEntry(EnsureModuleLocalExportEntryList(), nullptr, localName, localName, nullptr);
            }
        }
        break;

    case tkDEFAULT:
        {
            pnode = ParseDefaultExportClause<buildAST>();
        }
        break;

    default:
        {
ErrorToken:
            Error(ERRsyntax);
        }
    }

    return pnode;
}