GetSockStateName(SockState sockState)
{
    int sockStateInt = (int)sockState;
    static const char *sockStateStrings[] = {
        "SOCK_READY",
        "SOCK_MORE",
        "SOCK_SPOOL",
        "SOCK_ERROR",
        "SOCK_CLOSE",
        "SOCK_CLOSETIMEOUT",
        "SOCK_READTIMEOUT",
        "SOCK_WRITETIMEOUT",
        "SOCK_READERROR",
        "SOCK_WRITEERROR",
        "SOCK_SHUTERROR",
        "SOCK_BADREQUEST",
        "SOCK_ENTITYTOOLARGE",
        "SOCK_BADHEADER",
        "SOCK_TOOMANYHEADERS",
        NULL
    };

    if (sockStateInt < 0) {
        sockStateInt = (- sockStateInt) + 2;
    }
    assert(sockStateInt < Ns_NrElements(sockStateStrings));
    return sockStateStrings[sockStateInt];
}