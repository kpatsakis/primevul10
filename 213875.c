bool WindowsServiceControl::uninstall()
{
	if( checkService() == false )
	{
		return false;
	}

	if( stop() == false )
	{
		return false;
	}

	if( DeleteService( m_serviceHandle ) == false )
	{
		vCritical() << qUtf8Printable( tr( "The service \"%1\" could not be uninstalled." ).arg( m_name ) );
		return false;
	}

	vInfo() << qUtf8Printable( tr( "The service \"%1\" has been uninstalled successfully." ).arg( m_name ) );

	return true;
}