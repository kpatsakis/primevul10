xmlFreeEntityWrapper(void *entity, const xmlChar *name ATTRIBUTE_UNUSED) {
    if (entity != NULL)
	xmlFreeEntity((xmlEntityPtr) entity);
}