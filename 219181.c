rfbSetServerVersionIdentity(rfbScreenInfoPtr screen, char *fmt, ...)
{
    char buffer[256];
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
    va_end(ap);
    
    if (screen->versionString!=NULL) free(screen->versionString);
    screen->versionString = strdup(buffer);
}