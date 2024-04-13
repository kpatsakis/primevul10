static void CheckSectionsAllocated(void)
{
    if (SectionsRead > SectionsAllocated){
        ErrFatal("allocation screwup");
    }
    if (SectionsRead >= SectionsAllocated){
        SectionsAllocated += SectionsAllocated/2;
        Sections = (Section_t *)realloc(Sections, sizeof(Section_t)*SectionsAllocated);
        if (Sections == NULL){
            ErrFatal("could not allocate data for entire image");
        }
    }
}