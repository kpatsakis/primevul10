static MagickBooleanType DrawPolygonPrimitive(Image *image,
  const DrawInfo *draw_info,const PrimitiveInfo *primitive_info)
{
  CacheView
    *image_view;

  const char
    *artifact;

  double
    mid;

  ExceptionInfo
    *exception;

  MagickBooleanType
    fill,
    status;

  PolygonInfo
    **magick_restrict polygon_info;

  EdgeInfo
    *p;

  ssize_t
    i;

  SegmentInfo
    bounds;

  ssize_t
    start_y,
    stop_y,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickCoreSignature);
  assert(primitive_info != (PrimitiveInfo *) NULL);
  if (primitive_info->coordinates <= 1)
    return(MagickTrue);
  /*
    Compute bounding box.
  */
  polygon_info=AcquirePolygonThreadSet(draw_info,primitive_info,
    &image->exception);
  if (polygon_info == (PolygonInfo **) NULL)
    return(MagickFalse);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    begin draw-polygon");
  fill=(primitive_info->method == FillToBorderMethod) ||
    (primitive_info->method == FloodfillMethod) ? MagickTrue : MagickFalse;
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  bounds=polygon_info[0]->edges[0].bounds;
  artifact=GetImageArtifact(image,"draw:render-bounding-rectangles");
  if (IsStringTrue(artifact) != MagickFalse)
    (void) DrawBoundingRectangles(image,draw_info,polygon_info[0]);
  for (i=1; i < (ssize_t) polygon_info[0]->number_edges; i++)
  {
    p=polygon_info[0]->edges+i;
    if (p->bounds.x1 < bounds.x1)
      bounds.x1=p->bounds.x1;
    if (p->bounds.y1 < bounds.y1)
      bounds.y1=p->bounds.y1;
    if (p->bounds.x2 > bounds.x2)
      bounds.x2=p->bounds.x2;
    if (p->bounds.y2 > bounds.y2)
      bounds.y2=p->bounds.y2;
  }
  bounds.x1-=(mid+1.0);
  bounds.y1-=(mid+1.0);
  bounds.x2+=(mid+1.0);
  bounds.y2+=(mid+1.0);
  if ((bounds.x1 >= (double) image->columns) ||
      (bounds.y1 >= (double) image->rows) ||
      (bounds.x2 <= 0.0) || (bounds.y2 <= 0.0))
    {
      polygon_info=DestroyPolygonThreadSet(polygon_info);
      return(MagickTrue);  /* virtual polygon */
    }
  bounds.x1=bounds.x1 < 0.0 ? 0.0 : bounds.x1 >= (double) image->columns-1.0 ?
    (double) image->columns-1.0 : bounds.x1;
  bounds.y1=bounds.y1 < 0.0 ? 0.0 : bounds.y1 >= (double) image->rows-1.0 ?
    (double) image->rows-1.0 : bounds.y1;
  bounds.x2=bounds.x2 < 0.0 ? 0.0 : bounds.x2 >= (double) image->columns-1.0 ?
    (double) image->columns-1.0 : bounds.x2;
  bounds.y2=bounds.y2 < 0.0 ? 0.0 : bounds.y2 >= (double) image->rows-1.0 ?
    (double) image->rows-1.0 : bounds.y2;
  status=MagickTrue;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
  if ((primitive_info->coordinates == 1) ||
      (polygon_info[0]->number_edges == 0))
    {
      /*
        Draw point.
      */
      start_y=CastDoubleToLong(ceil(bounds.y1-0.5));
      stop_y=CastDoubleToLong(floor(bounds.y2+0.5));
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static) shared(status) \
        magick_number_threads(image,image,stop_y-start_y+1,1)
#endif
      for (y=start_y; y <= stop_y; y++)
      {
        MagickBooleanType
          sync;

        PixelPacket
          *magick_restrict q;

        ssize_t
          x;

        ssize_t
          start_x,
          stop_x;

        if (status == MagickFalse)
          continue;
        start_x=CastDoubleToLong(ceil(bounds.x1-0.5));
        stop_x=CastDoubleToLong(floor(bounds.x2+0.5));
        x=start_x;
        q=GetCacheViewAuthenticPixels(image_view,x,y,(size_t) (stop_x-x+1),1,
          exception);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        for ( ; x <= stop_x; x++)
        {
          if ((x == CastDoubleToLong(ceil(primitive_info->point.x-0.5))) &&
              (y == CastDoubleToLong(ceil(primitive_info->point.y-0.5))))
            (void) GetFillColor(draw_info,x-start_x,y-start_y,q);
          q++;
        }
        sync=SyncCacheViewAuthenticPixels(image_view,exception);
        if (sync == MagickFalse)
          status=MagickFalse;
      }
      image_view=DestroyCacheView(image_view);
      polygon_info=DestroyPolygonThreadSet(polygon_info);
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(DrawEvent,GetMagickModule(),
          "    end draw-polygon");
      return(status);
    }
  /*
    Draw polygon or line.
  */
  start_y=CastDoubleToLong(ceil(bounds.y1-0.5));
  stop_y=CastDoubleToLong(floor(bounds.y2+0.5));
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,stop_y-start_y+1,1)
#endif
  for (y=start_y; y <= stop_y; y++)
  {
    const int
      id = GetOpenMPThreadId();

    double
      fill_opacity,
      stroke_opacity;

    PixelPacket
      fill_color,
      stroke_color;

    PixelPacket
      *magick_restrict q;

    ssize_t
      x;

    ssize_t
      start_x,
      stop_x;

    if (status == MagickFalse)
      continue;
    start_x=CastDoubleToLong(ceil(bounds.x1-0.5));
    stop_x=CastDoubleToLong(floor(bounds.x2+0.5));
    q=GetCacheViewAuthenticPixels(image_view,start_x,y,(size_t) (stop_x-start_x+
      1),1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=start_x; x <= stop_x; x++)
    {
      /*
        Fill and/or stroke.
      */
      fill_opacity=GetOpacityPixel(polygon_info[id],mid,fill,
        draw_info->fill_rule,x,y,&stroke_opacity);
      if (draw_info->stroke_antialias == MagickFalse)
        {
          fill_opacity=fill_opacity > 0.5 ? 1.0 : 0.0;
          stroke_opacity=stroke_opacity > 0.5 ? 1.0 : 0.0;
        }
      (void) GetFillColor(draw_info,x-start_x,y-start_y,&fill_color);
      fill_opacity=(double) (QuantumRange-fill_opacity*(QuantumRange-
        fill_color.opacity));
      MagickCompositeOver(&fill_color,(MagickRealType) fill_opacity,q,
        (MagickRealType) q->opacity,q);
      (void) GetStrokeColor(draw_info,x-start_x,y-start_y,&stroke_color);
      stroke_opacity=(double) (QuantumRange-stroke_opacity*(QuantumRange-
        stroke_color.opacity));
      MagickCompositeOver(&stroke_color,(MagickRealType) stroke_opacity,q,
        (MagickRealType) q->opacity,q);
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  polygon_info=DestroyPolygonThreadSet(polygon_info);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    end draw-polygon");
  return(status);
}