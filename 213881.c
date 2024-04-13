bool WindowsServiceControl::start()
{
	if( checkService() == false )
	{
		return false;
	}

	SERVICE_STATUS status;
	status.dwCurrentState = SERVICE_START_PENDING;

	if( StartService( m_serviceHandle, 0, nullptr ) )
	{
		while( QueryServiceStatus( m_serviceHandle, &status ) )
		{
			if( status.dwCurrentState == SERVICE_START_PENDING )
			{
				Sleep( 1000 );
			}
			else
			{
				break;
			}
		}
	}

	if( status.dwCurrentState != SERVICE_RUNNING )
	{
		vWarning() << "service" << m_name << "could not be started.";
		return false;
	}

	return true;
}