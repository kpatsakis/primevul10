int KPasswordDlg::tryPassword()
{
#if defined HAVE_ETCPASSWD || defined HAVE_SHADOW || defined HAVE_PAM
	if( stars )
	  blinkTimer->stop();
	KProcess chkpass;
	QString kcp_binName = "";
	kcp_binName += KApplication::kde_bindir();
	kcp_binName += "/kcheckpass";
	chkpass.clearArguments();
	chkpass << kcp_binName;
	bool ret = chkpass.start(KProcess::DontCare, KProcess::Stdin);
	if (ret == false) {
	  if( stars )
	    blinkTimer->start( 300 );
	  return 2;
	}
        chkpass.writeStdin(password.data(),password.length()); // write Password to stdin
	chkpass.closeStdin();                // eof

	int timeout = 1000;
	while ( timeout != 0 ) {
	  if (! chkpass.isRunning() )
	    break;
	  else {
	    globalKapp->processEvents();
	    timeout--;
	    usleep(10000);
	  }
	}
	
	int rc = ( chkpass.normalExit() && (chkpass.exitStatus() == 0) );
	if( stars )
	  blinkTimer->start( 300 ); 
	return rc;
#else
	int e = checkPasswd(password.data());
	return e;
#endif
}