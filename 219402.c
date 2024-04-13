xmlFreeEntitiesTable(xmlEntitiesTablePtr table) {
    xmlHashFree(table, xmlFreeEntityWrapper);
}