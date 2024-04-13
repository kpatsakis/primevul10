Section_t * FindSection(int SectionType)
{
    int a;

    for (a=0;a<SectionsRead;a++){
        if (Sections[a].Type == SectionType){
            return &Sections[a];
        }
    }
    // Could not be found.
    return NULL;
}