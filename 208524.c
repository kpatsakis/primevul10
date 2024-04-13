Parser::~Parser(void)
{
    if (m_scriptContext == nullptr || m_scriptContext->GetGuestArena() == nullptr)
    {
        // If the scriptContext or guestArena have gone away, there is no point clearing each item of this list.
        // Just reset it so that destructor of the SList will be no-op
        m_registeredRegexPatterns.Reset();
    }

    if (this->m_hasParallelJob)
    {
#if ENABLE_BACKGROUND_PARSING
        // Let the background threads know that they can decommit their arena pages.
        BackgroundParser *bgp = m_scriptContext->GetBackgroundParser();
        Assert(bgp);
        if (bgp->Processor()->ProcessesInBackground())
        {
            JsUtil::BackgroundJobProcessor *processor = static_cast<JsUtil::BackgroundJobProcessor*>(bgp->Processor());

            bool result = processor->IterateBackgroundThreads([&](JsUtil::ParallelThreadData *threadData)->bool {
                threadData->canDecommit = true;
                return false;
            });
            Assert(result);
        }
#endif
    }

    Release();

}