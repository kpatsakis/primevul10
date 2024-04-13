PLT_HttpServer::PLT_HttpServer(NPT_IpAddress address,
                               NPT_IpPort    port,
                               bool          allow_random_port_on_bind_failure,   /* = false */
                               NPT_Cardinal  max_clients,                         /* = 50 */
                               bool          reuse_address) :                     /* = false */
    NPT_HttpServer(address, port, true),
    m_TaskManager(new PLT_TaskManager(max_clients)),
    m_Address(address),
    m_Port(port),
    m_AllowRandomPortOnBindFailure(allow_random_port_on_bind_failure),
    m_ReuseAddress(reuse_address),
    m_Running(false),
    m_Aborted(false)
{
}