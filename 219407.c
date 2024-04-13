xmlDumpEntityDeclScan(void *ent, void *buf,
                      const xmlChar *name ATTRIBUTE_UNUSED) {
    xmlDumpEntityDecl((xmlBufferPtr) buf, (xmlEntityPtr) ent);
}