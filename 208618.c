void Parser::WaitForBackgroundJobs(BackgroundParser *bgp, CompileScriptException *pse)
{
    // The scan of the script is done, but there may be unfinished background jobs in the queue.
    // Enlist the main thread to help with those.
    BackgroundParseItem *item;
    if (!*bgp->GetPendingBackgroundItemsPtr())
    {
        // We're done.
        return;
    }

    // Save parser state, since we'll need to restore it in order to bind references correctly later.
    this->m_isInBackground = true;
    this->SetCurrBackgroundParseItem(nullptr);
    uint blockIdSave = this->m_nextBlockId;
    uint functionIdSave = *this->m_nextFunctionId;
    StmtNest *pstmtSave = this->m_pstmtCur;

    if (!bgp->Processor()->ProcessesInBackground())
    {
        // No background thread. Just walk the jobs with no locking and process them.
        for (item = bgp->GetNextUnprocessedItem(); item; item = bgp->GetNextUnprocessedItem())
        {
            bgp->Processor()->RemoveJob(item);
            bool succeeded = bgp->Process(item, this, pse);
            bgp->JobProcessed(item, succeeded);
        }
        Assert(!*bgp->GetPendingBackgroundItemsPtr());
    }
    else
    {
        // Background threads. We need to have the critical section in order to:
        // - Check for unprocessed jobs;
        // - Remove jobs from the processor queue;
        // - Do JobsProcessed work (such as removing jobs from the BackgroundParser's unprocessed list).
        CriticalSection *pcs = static_cast<JsUtil::BackgroundJobProcessor*>(bgp->Processor())->GetCriticalSection();
        pcs->Enter();
        for (;;)
        {
            // Grab a job (in lock)
            item = bgp->GetNextUnprocessedItem();
            if (item == nullptr)
            {
                break;
            }
            bgp->Processor()->RemoveJob(item);
            pcs->Leave();

            // Process job (if there is one) (outside lock)
            bool succeeded = bgp->Process(item, this, pse);

            pcs->Enter();
            bgp->JobProcessed(item, succeeded);
        }
        pcs->Leave();

        // Wait for the background threads to finish jobs they're already processing (if any).
        // TODO: Replace with a proper semaphore.
        while(*bgp->GetPendingBackgroundItemsPtr());
    }

    Assert(!*bgp->GetPendingBackgroundItemsPtr());

    // Restore parser state.
    this->m_pstmtCur = pstmtSave;
    this->m_isInBackground = false;
    this->m_nextBlockId = blockIdSave;
    *this->m_nextFunctionId = functionIdSave;
}