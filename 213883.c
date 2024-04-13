int WindowsServiceControl::startType()
{
	if( checkService() == false )
	{
		return InvalidStartType;
	}

	LPQUERY_SERVICE_CONFIG serviceConfig{nullptr};
	DWORD bufferSize = 0;
	DWORD bytesNeeded = 0;

	if( QueryServiceConfig( m_serviceHandle, nullptr, 0, &bytesNeeded ) == false )
	{
		if( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		{
			bufferSize = bytesNeeded;
			serviceConfig = LPQUERY_SERVICE_CONFIG(LocalAlloc(LMEM_FIXED, bufferSize));
		}
		else
		{
			return InvalidStartType;
		}
	}
	else
	{
		return InvalidStartType;
	}

	if( QueryServiceConfig( m_serviceHandle, serviceConfig, bufferSize, &bytesNeeded ) == false )
	{
		const auto error = GetLastError();
		vCritical() << error;
		LocalFree( serviceConfig );
		return InvalidStartType;
	}

	const auto startType = serviceConfig->dwStartType;

	LocalFree( serviceConfig );

	return startType;
}