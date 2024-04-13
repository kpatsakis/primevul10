WindowsServiceControl::~WindowsServiceControl()
{
	CloseServiceHandle( m_serviceHandle );
	CloseServiceHandle( m_serviceManager );
}