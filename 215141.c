void ResetJpgfile(void)
{
    if (Sections == NULL){
        Sections = (Section_t *)malloc(sizeof(Section_t)*5);
        SectionsAllocated = 5;
    }

    SectionsRead = 0;
    HaveAll = 0;
}