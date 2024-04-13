PLT_HttpServer::Stop()
{
    // we can't restart an aborted server
    if (m_Aborted || !m_Running) NPT_CHECK_WARNING(NPT_ERROR_INVALID_STATE);
    
    // stop all other pending tasks 
    m_TaskManager->Abort();
    
    m_Running = false;
    m_Aborted = true;
    
    return NPT_SUCCESS;
}