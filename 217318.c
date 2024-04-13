static Image *AcquireImageCanvas(const Image *images,ExceptionInfo *exception)
{
  const Image
    *p,
    *q;

  size_t
    columns,
    number_channels,
    rows;

  q=images;
  columns=images->columns;
  rows=images->rows;
  number_channels=0;
  for (p=images; p != (Image *) NULL; p=p->next)
  {
    size_t
      channels;

    channels=3;
    if (p->matte != MagickFalse)
      channels+=1;
    if (p->colorspace == CMYKColorspace)
      channels+=1;
    if (channels > number_channels)
      {
        number_channels=channels;
        q=p;
      }
    if (p->columns > columns)
      columns=p->columns;
    if (p->rows > rows)
      rows=p->rows;
  }
  return(CloneImage(q,columns,rows,MagickTrue,exception));
}