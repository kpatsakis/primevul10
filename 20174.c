static void ipa_bmp_free(wmfAPI * API, wmfBMP * bmp)
{
  (void) API;
  DestroyImageList((Image*)bmp->data);
  bmp->data = (void*) 0;
  bmp->width = (U16) 0;
  bmp->height = (U16) 0;
}