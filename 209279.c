PLT_HttpServer::Start()
{
    NPT_Result res = NPT_FAILURE;
    
    // we can't start an already running server or restart an aborted server
    // because the socket is shared create a new instance
    if (m_Running || m_Aborted) NPT_CHECK_WARNING(NPT_ERROR_INVALID_STATE);
    
    // if we're given a port for our http server, try it
    if (m_Port) {
        res = SetListenPort(m_Port, m_ReuseAddress);
        // return right away if failed and not allowed to try again randomly
        if (NPT_FAILED(res) && !m_AllowRandomPortOnBindFailure) {
            NPT_CHECK_SEVERE(res);
        }
    }
    
    // try random port now
    if (!m_Port || NPT_FAILED(res)) {
        int retries = 100;
        do {    
            int random = NPT_System::GetRandomInteger();
            int port = (unsigned short)(1024 + (random % 1024));
            if (NPT_SUCCEEDED(SetListenPort(port, m_ReuseAddress))) {
                break;
            }
        } while (--retries > 0);

        if (retries == 0) NPT_CHECK_SEVERE(NPT_FAILURE);
    }

    // keep track of port server has successfully bound
    m_Port = m_BoundPort;

    // Tell server to try to listen to more incoming sockets
    // (this could fail silently)
    if (m_TaskManager->GetMaxTasks() > 20) {
        m_Socket.Listen(m_TaskManager->GetMaxTasks());
    }
    
    // start a task to listen for incoming connections
    PLT_HttpListenTask *task = new PLT_HttpListenTask(this, &m_Socket, false);
    NPT_CHECK_SEVERE(m_TaskManager->StartTask(task));

    NPT_SocketInfo info;
    m_Socket.GetInfo(info);
    NPT_LOG_INFO_2("HttpServer listening on %s:%d", 
        (const char*)info.local_address.GetIpAddress().ToString(), 
        m_Port);
    
    m_Running = true;
    return NPT_SUCCESS;
}