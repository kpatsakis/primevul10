rfbBool rfbFilenameTranslate2DOS(rfbClientPtr cl, char *unixPath, char *path)
{
    int x;

    FILEXFER_ALLOWED_OR_CLOSE_AND_RETURN("", cl, FALSE);

    sprintf(path,"C:%s", unixPath);
    for (x=2;x<strlen(path);x++)
        if (path[x]=='/') path[x]='\\';
    return TRUE;
}