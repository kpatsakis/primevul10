int mp4read_close(void)
{
#define FREE(x) if(x){free(x);x=0;}
    FREE(mp4config.frame.data);
    FREE(mp4config.bitbuf.data);

    return ERR_OK;
}