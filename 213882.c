WindowsServiceControl::WindowsServiceControl( const QString& name ) :
	m_name( name ),
	m_serviceManager( nullptr ),
	m_serviceHandle( nullptr )
{
	m_serviceManager = OpenSCManager( nullptr, nullptr, SC_MANAGER_ALL_ACCESS );

	if( m_serviceManager )
	{
		m_serviceHandle = OpenService( m_serviceManager, WindowsCoreFunctions::toConstWCharArray( m_name ),
									   SERVICE_ALL_ACCESS );
		if( m_serviceHandle == nullptr )
		{
			vCritical() << "could not open service" << m_name;
		}
	}
	else
	{
		vCritical() << "the Service Control Manager could not be contacted - service " << m_name << "can't be controlled.";
	}
}