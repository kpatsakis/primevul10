int ReadJpegFile(const char * FileName, ReadMode_t ReadMode)
{
    FILE * infile;
    int ret;

    infile = fopen(FileName, "rb"); // Unix ignores 'b', windows needs it.

    if (infile == NULL) {
        fprintf(stderr, "can't open '%s'\n", FileName);
        return FALSE;
    }


    // Scan the JPEG headers.
    ret = ReadJpegSections(infile, ReadMode);
    if (!ret){
        if (ReadMode == READ_ANY){
            // Process any files mode.  Ignore the fact that it's not
            // a jpeg file.
            ret = TRUE;
        }else{
            fprintf(stderr,"Not JPEG: %s\n",FileName);
        }
    }

    fclose(infile);

    if (ret == FALSE){
        DiscardData();
    }
    return ret;
}