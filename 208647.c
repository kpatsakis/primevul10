void Parser::BindPidRefs(BlockInfoStack *blockInfo, uint maxBlockId)
{
    // We need to bind all assignments in order to emit assignment to 'const' error
    int blockId = blockInfo->pnodeBlock->sxBlock.blockId;

    Scope *scope = blockInfo->pnodeBlock->sxBlock.scope;
    if (scope)
    {
        auto bindPidRefs = [blockId, maxBlockId, this](Symbol *sym)
        {
            ParseNodePtr pnode = sym->GetDecl();
            IdentPtr pid;
#if PROFILE_DICTIONARY
            int depth = 0;
#endif
            Assert(pnode);
            switch (pnode->nop)
            {
            case knopVarDecl:
            case knopLetDecl:
            case knopConstDecl:
                pid = pnode->sxVar.pid;
                if (backgroundPidRef)
                {
                    pid = this->m_pscan->m_phtbl->FindExistingPid(pid->Psz(), pid->Psz() + pid->Cch(), pid->Cch(), pid->Hash(), nullptr, nullptr
#if PROFILE_DICTIONARY
                                                                  , depth
#endif
                        );
                    if (pid == nullptr)
                    {
                        break;
                    }
                }
                this->BindPidRefsInScope(pid, sym, blockId, maxBlockId);
                break;
            case knopName:
                pid = pnode->sxPid.pid;
                if (backgroundPidRef)
                {
                    pid = this->m_pscan->m_phtbl->FindExistingPid(pid->Psz(), pid->Psz() + pid->Cch(), pid->Cch(), pid->Hash(), nullptr, nullptr
#if PROFILE_DICTIONARY
                                                                  , depth
#endif
                        );
                    if (pid == nullptr)
                    {
                        break;
                    }
                }
                this->BindPidRefsInScope(pid, sym, blockId, maxBlockId);
                break;
            default:
                Assert(0);
                break;
            }
        };

        scope->ForEachSymbol(bindPidRefs);
    }
}