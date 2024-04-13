xmlCopyEntitiesTable(xmlEntitiesTablePtr table) {
    return(xmlHashCopy(table, xmlCopyEntity));
}