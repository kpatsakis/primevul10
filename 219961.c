static Image *DrawClippingMask(Image *image,const DrawInfo *draw_info,
  const char *id,const char *clip_path,ExceptionInfo *exception)
{
  DrawInfo
    *clone_info;

  Image
    *clip_mask;

  MagickStatusType
    status;

  /*
    Draw a clip path.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (const DrawInfo *) NULL);
  clip_mask=AcquireImage((const ImageInfo *) NULL);
  status=SetImageExtent(clip_mask,image->columns,image->rows);
  if (status == MagickFalse)
    return(DestroyImage(clip_mask));
  status=SetImageClipMask(image,(Image *) NULL);
  status=QueryColorCompliance("#0000",AllCompliance,
    &clip_mask->background_color,exception);
  clip_mask->background_color.opacity=(Quantum) TransparentOpacity;
  status=SetImageBackgroundColor(clip_mask);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"\nbegin clip-path %s",
      id);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,clip_path);
  status=QueryColorCompliance("#ffffff",AllCompliance,&clone_info->fill,
    exception);
  if (clone_info->clip_mask != (char *) NULL)
    clone_info->clip_mask=DestroyString(clone_info->clip_mask);
  (void) QueryColorCompliance("#00000000",AllCompliance,&clone_info->stroke,
    exception);
  clone_info->stroke_width=0.0;
  clone_info->opacity=OpaqueOpacity;
  clone_info->clip_path=MagickTrue;
  status=RenderMVGContent(clip_mask,clone_info,0);
  clone_info=DestroyDrawInfo(clone_info);
  status&=SeparateImageChannel(clip_mask,TrueAlphaChannel);
  if (draw_info->compliance != SVGCompliance)
    status&=NegateImage(clip_mask,MagickFalse);
  if (status == MagickFalse)
    clip_mask=DestroyImage(clip_mask);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"end clip-path");
  return(clip_mask);
}