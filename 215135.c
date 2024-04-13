int SaveThumbnail(char * ThumbFileName)
{
    FILE * ThumbnailFile;

    if (ImageInfo.ThumbnailOffset == 0 || ImageInfo.ThumbnailSize == 0){
        fprintf(stderr,"Image contains no thumbnail\n");
        return FALSE;
    }

    if (strcmp(ThumbFileName, "-") == 0){
        // A filename of '-' indicates thumbnail goes to stdout.
        // This doesn't make much sense under Windows, so this feature is unix only.
        ThumbnailFile = stdout;
    }else{
        ThumbnailFile = fopen(ThumbFileName,"wb");
    }

    if (ThumbnailFile){
        uchar * ThumbnailPointer;
        Section_t * ExifSection;
        ExifSection = FindSection(M_EXIF);
        ThumbnailPointer = ExifSection->Data+ImageInfo.ThumbnailOffset+8;

        fwrite(ThumbnailPointer, ImageInfo.ThumbnailSize ,1, ThumbnailFile);
        fclose(ThumbnailFile);
        return TRUE;
    }else{
        ErrFatal("Could not write thumbnail file");
        return FALSE;
    }
}