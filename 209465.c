static char *dbLocalGetline(Jsi_Interp *interp, char *zPrompt, Jsi_Channel in) {
    char *zLine;
    int nLine;
    int n;
    int eol;

    nLine = 100;
    zLine = (char*)Jsi_Malloc( nLine );
    if( zLine==0 ) return 0;
    n = 0;
    eol = 0;
    while( !eol ) {
        if( n+100>nLine ) {
            nLine = nLine*2 + 100;
            zLine = (char*)Jsi_Realloc(zLine, nLine);
            if( zLine==0 ) return 0;
        }
        if( Jsi_Gets(interp, in, &zLine[n], nLine - n)==0 ) {
            if( n==0 ) {
                Jsi_Free(zLine);
                return 0;
            }
            zLine[n] = 0;
            eol = 1;
            break;
        }
        while( zLine[n] ) {
            n++;
        }
        if( n>0 && zLine[n-1]=='\n' ) {
            n--;
            zLine[n] = 0;
            eol = 1;
        }
    }
    zLine = (char*)Jsi_Realloc( zLine, n+1 );
    return zLine;
}