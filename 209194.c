AsyncWriterRelease(AsyncWriteData *wdPtr)
{
    NS_NONNULL_ASSERT(wdPtr != NULL);

    ns_free(wdPtr->data);
    ns_free(wdPtr);
}