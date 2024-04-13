KCleanup::KCleanup( const KCleanup &src )
    : KAction(src.title(), src.parent())
{
    copy( src );
}