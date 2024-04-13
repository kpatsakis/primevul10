bool WindowsServiceControl::isRunning()
{
	if( checkService() == false )
	{
		return false;
	}

	SERVICE_STATUS status;
	if( QueryServiceStatus( m_serviceHandle, &status ) )
	{
		return status.dwCurrentState == SERVICE_RUNNING;
	}

	return false;
}