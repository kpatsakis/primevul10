static void xmlSHRINK (xmlParserCtxtPtr ctxt) {
    xmlParserInputShrink(ctxt->input);
    if (*ctxt->input->cur == 0)
        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
}