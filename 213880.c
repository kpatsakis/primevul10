bool WindowsServiceControl::stop()
{
	if( checkService() == false )
	{
		return false;
	}

	SERVICE_STATUS status;

	// Try to stop the service
	if( ControlService( m_serviceHandle, SERVICE_CONTROL_STOP, &status ) )
	{
		while( QueryServiceStatus( m_serviceHandle, &status ) )
		{
			if( status.dwCurrentState == SERVICE_STOP_PENDING )
			{
				Sleep( 1000 );
			}
			else
			{
				break;
			}
		}

		if( status.dwCurrentState != SERVICE_STOPPED )
		{
			vWarning() << "service" << m_name << "could not be stopped.";
			return false;
		}
	}

	return true;
}