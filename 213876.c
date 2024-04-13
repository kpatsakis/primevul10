bool WindowsServiceControl::checkService() const
{
	if( m_serviceHandle == nullptr )
	{
		vCritical() << qUtf8Printable( tr( "Service \"%1\" could not be found." ).arg( m_name ) );
		return false;
	}

	return true;
}