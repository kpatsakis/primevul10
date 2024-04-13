void Con_DrawConsole( void ) {
	// check for console width changes from a vid mode change
	Con_CheckResize();

	// if disconnected, render console full screen
	switch ( clc.state ) {
	case CA_UNINITIALIZED:
	case CA_CONNECTING:         // sending request packets to the server
	case CA_CHALLENGING:        // sending challenge packets to the server
	case CA_CONNECTED:          // netchan_t established, getting gamestate
	case CA_PRIMED:             // got gamestate, waiting for first frame
	case CA_LOADING:            // only during cgame initialization, never during main loop
		if ( !con_debug->integer ) { // these are all 'no console at all' when con_debug is not set
			return;
		}

		if ( Key_GetCatcher( ) & KEYCATCH_UI ) {
			return;
		}

		Con_DrawSolidConsole( 1.0 );
		return;

	case CA_DISCONNECTED:       // not talking to a server
		if ( !( Key_GetCatcher( ) & KEYCATCH_UI ) ) {
			Con_DrawSolidConsole( 1.0 );
			return;
		}
		break;

	case CA_ACTIVE:             // game views should be displayed
		if ( con.displayFrac ) {
			if ( con_debug->integer == 2 ) {    // 2 means draw full screen console at '~'
//					Con_DrawSolidConsole( 1.0f );
				Con_DrawSolidConsole( con.displayFrac * 2.0f );
				return;
			}
		}

		break;


	case CA_CINEMATIC:          // playing a cinematic or a static pic, not connected to a server
	default:
		break;
	}

	if ( con.displayFrac ) {
		Con_DrawSolidConsole( con.displayFrac );
	} else {
		Con_DrawNotify();       // draw notify lines
	}
}