void DiscardData(void)
{
    int a;

    for (a=0;a<SectionsRead;a++){
        free(Sections[a].Data);
    }

    memset(&ImageInfo, 0, sizeof(ImageInfo));
    SectionsRead = 0;
    HaveAll = 0;
}