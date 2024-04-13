MagickExport MagickBooleanType DrawPrimitive(Image *image,
  const DrawInfo *draw_info,const PrimitiveInfo *primitive_info)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  MagickStatusType
    status;

  ssize_t
    i,
    x;

  ssize_t
    y;

  if (image->debug != MagickFalse)
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "  begin draw-primitive");
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "    affine: %g,%g,%g,%g,%g,%g",draw_info->affine.sx,
        draw_info->affine.rx,draw_info->affine.ry,draw_info->affine.sy,
        draw_info->affine.tx,draw_info->affine.ty);
    }
  exception=(&image->exception);
  status=MagickTrue;
  if ((IsGrayColorspace(image->colorspace) != MagickFalse) &&
      ((IsPixelGray(&draw_info->fill) == MagickFalse) ||
       (IsPixelGray(&draw_info->stroke) == MagickFalse)))
    status=SetImageColorspace(image,sRGBColorspace);
  if (draw_info->compliance == SVGCompliance)
    {
      status&=SetImageClipMask(image,draw_info->clipping_mask);
      status&=SetImageMask(image,draw_info->composite_mask);
    }
  x=CastDoubleToLong(ceil(primitive_info->point.x-0.5));
  y=CastDoubleToLong(ceil(primitive_info->point.y-0.5));
  image_view=AcquireAuthenticCacheView(image,exception);
  switch (primitive_info->primitive)
  {
    case ColorPrimitive:
    {
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          PixelPacket
            *q;

          q=GetCacheViewAuthenticPixels(image_view,x,y,1,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          (void) GetFillColor(draw_info,x,y,q);
          status&=SyncCacheViewAuthenticPixels(image_view,exception);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          status&=GetOneCacheViewVirtualPixel(image_view,x,y,&target,exception);
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            PixelPacket
              *magick_restrict q;

            q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
              exception);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (IsColorSimilar(image,q,&target) == MagickFalse)
                {
                  q++;
                  continue;
                }
              (void) GetFillColor(draw_info,x,y,q);
              q++;
            }
            status&=SyncCacheViewAuthenticPixels(image_view,exception);
            if (status == MagickFalse)
              break;
          }
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          MagickPixelPacket
            target;

          status&=GetOneVirtualMagickPixel(image,x,y,&target,exception);
          if (primitive_info->method == FillToBorderMethod)
            {
              target.red=(MagickRealType) draw_info->border_color.red;
              target.green=(MagickRealType) draw_info->border_color.green;
              target.blue=(MagickRealType) draw_info->border_color.blue;
            }
          status&=FloodfillPaintImage(image,DefaultChannels,draw_info,&target,x,
            y,primitive_info->method == FloodfillMethod ? MagickFalse :
            MagickTrue);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            PixelPacket
              *magick_restrict q;

            ssize_t
              x;

            q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
              exception);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              (void) GetFillColor(draw_info,x,y,q);
              q++;
            }
            status&=SyncCacheViewAuthenticPixels(image_view,exception);
            if (status == MagickFalse)
              break;
          }
          break;
        }
      }
      break;
    }
    case MattePrimitive:
    {
      if (image->matte == MagickFalse)
        status&=SetImageAlphaChannel(image,OpaqueAlphaChannel);
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          PixelPacket
            pixel;

          PixelPacket
            *q;

          q=GetCacheViewAuthenticPixels(image_view,x,y,1,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          (void) GetFillColor(draw_info,x,y,&pixel);
          SetPixelOpacity(q,pixel.opacity);
          status&=SyncCacheViewAuthenticPixels(image_view,exception);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            pixel,
            target;

          status&=GetOneCacheViewVirtualPixel(image_view,x,y,&target,exception);
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            PixelPacket
              *magick_restrict q;

            ssize_t
              x;

            q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
              exception);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (IsColorSimilar(image,q,&target) == MagickFalse)
                {
                  q++;
                  continue;
                }
              (void) GetFillColor(draw_info,x,y,&pixel);
              SetPixelOpacity(q,pixel.opacity);
              q++;
            }
            status&=SyncCacheViewAuthenticPixels(image_view,exception);
            if (status == MagickFalse)
              break;
          }
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          MagickPixelPacket
            target;

          status&=GetOneVirtualMagickPixel(image,x,y,&target,exception);
          if (primitive_info->method == FillToBorderMethod)
            {
              target.red=(MagickRealType) draw_info->border_color.red;
              target.green=(MagickRealType) draw_info->border_color.green;
              target.blue=(MagickRealType) draw_info->border_color.blue;
            }
          status&=FloodfillPaintImage(image,OpacityChannel,draw_info,&target,x,
            y,primitive_info->method == FloodfillMethod ? MagickFalse :
            MagickTrue);
          break;
        }
        case ResetMethod:
        {
          PixelPacket
            pixel;

          for (y=0; y < (ssize_t) image->rows; y++)
          {
            PixelPacket
              *magick_restrict q;

            ssize_t
              x;

            q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
              exception);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              (void) GetFillColor(draw_info,x,y,&pixel);
              SetPixelOpacity(q,pixel.opacity);
              q++;
            }
            status&=SyncCacheViewAuthenticPixels(image_view,exception);
            if (status == MagickFalse)
              break;
          }
          break;
        }
      }
      break;
    }
    case ImagePrimitive:
    {
      AffineMatrix
        affine;

      char
        composite_geometry[MaxTextExtent];

      Image
        *composite_image,
        *composite_images;

      ImageInfo
        *clone_info;

      RectangleInfo
        geometry;

      ssize_t
        x1,
        y1;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=AcquireImageInfo();
      composite_images=(Image *) NULL;
      if (LocaleNCompare(primitive_info->text,"data:",5) == 0)
        composite_images=ReadInlineImage(clone_info,primitive_info->text,
          &image->exception);
      else
        if (*primitive_info->text != '\0')
          {
            (void) CopyMagickString(clone_info->filename,primitive_info->text,
              MagickPathExtent);
            status&=SetImageInfo(clone_info,0,exception);
            if (clone_info->size != (char *) NULL)
              clone_info->size=DestroyString(clone_info->size);
            if (clone_info->extract != (char *) NULL)
              clone_info->extract=DestroyString(clone_info->extract);
            if ((LocaleNCompare(clone_info->magick,"http",4) == 0) ||
                (LocaleCompare(clone_info->magick,"mpri") == 0))
              (void) CopyMagickString(clone_info->filename,primitive_info->text,
                MagickPathExtent);
            if (*clone_info->filename != '\0')
              composite_images=ReadImage(clone_info,exception);
          }
      clone_info=DestroyImageInfo(clone_info);
      if (composite_images == (Image *) NULL)
        {
          status=0;
          break;
        }
      composite_image=RemoveFirstImageFromList(&composite_images);
      composite_images=DestroyImageList(composite_images);
      (void) SetImageProgressMonitor(composite_image,(MagickProgressMonitor)
        NULL,(void *) NULL);
      x1=CastDoubleToLong(ceil(primitive_info[1].point.x-0.5));
      y1=CastDoubleToLong(ceil(primitive_info[1].point.y-0.5));
      if (((x1 != 0L) && (x1 != (ssize_t) composite_image->columns)) ||
          ((y1 != 0L) && (y1 != (ssize_t) composite_image->rows)))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          (void) FormatLocaleString(geometry,MaxTextExtent,"%gx%g!",
            primitive_info[1].point.x,primitive_info[1].point.y);
          composite_image->filter=image->filter;
          status&=TransformImage(&composite_image,(char *) NULL,geometry);
        }
      if (composite_image->matte == MagickFalse)
        status&=SetImageAlphaChannel(composite_image,OpaqueAlphaChannel);
      if (draw_info->opacity != OpaqueOpacity)
        status&=SetImageOpacity(composite_image,draw_info->opacity);
      SetGeometry(image,&geometry);
      image->gravity=draw_info->gravity;
      geometry.x=x;
      geometry.y=y;
      (void) FormatLocaleString(composite_geometry,MaxTextExtent,
        "%.20gx%.20g%+.20g%+.20g",(double) composite_image->columns,(double)
        composite_image->rows,(double) geometry.x,(double) geometry.y);
      (void) ParseGravityGeometry(image,composite_geometry,&geometry,
        &image->exception);
      affine=draw_info->affine;
      affine.tx=(double) geometry.x;
      affine.ty=(double) geometry.y;
      composite_image->interpolate=image->interpolate;
      if ((draw_info->compose == OverCompositeOp) ||
          (draw_info->compose == SrcOverCompositeOp))
        status&=DrawAffineImage(image,composite_image,&affine);
      else
        status&=CompositeImage(image,draw_info->compose,composite_image,
          geometry.x,geometry.y);
      composite_image=DestroyImage(composite_image);
      break;
    }
    case PointPrimitive:
    {
      PixelPacket
        fill_color;

      PixelPacket
        *q;

      if ((y < 0) || (y >= (ssize_t) image->rows))
        break;
      if ((x < 0) || (x >= (ssize_t) image->columns))
        break;
      q=GetCacheViewAuthenticPixels(image_view,x,y,1,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      (void) GetFillColor(draw_info,x,y,&fill_color);
      MagickCompositeOver(&fill_color,(MagickRealType) fill_color.opacity,q,
        (MagickRealType) q->opacity,q);
      status&=SyncCacheViewAuthenticPixels(image_view,exception);
      break;
    }
    case TextPrimitive:
    {
      char
        geometry[MaxTextExtent];

      DrawInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
      (void) CloneString(&clone_info->text,primitive_info->text);
      (void) FormatLocaleString(geometry,MaxTextExtent,"%+f%+f",
        primitive_info->point.x,primitive_info->point.y);
      (void) CloneString(&clone_info->geometry,geometry);
      status&=AnnotateImage(image,clone_info);
      clone_info=DestroyDrawInfo(clone_info);
      break;
    }
    default:
    {
      double
        mid,
        scale;

      DrawInfo
        *clone_info;

      if (IsEventLogging() != MagickFalse)
        LogPrimitiveInfo(primitive_info);
      scale=ExpandAffine(&draw_info->affine);
      if ((draw_info->dash_pattern != (double *) NULL) &&
          (fabs(draw_info->dash_pattern[0]) >= MagickEpsilon) &&
          (fabs(scale*draw_info->stroke_width) >= MagickEpsilon) &&
          (draw_info->stroke.opacity != (Quantum) TransparentOpacity))
        {
          /*
            Draw dash polygon.
          */
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=(Quantum) TransparentOpacity;
          status&=DrawPolygonPrimitive(image,clone_info,primitive_info);
          clone_info=DestroyDrawInfo(clone_info);
          if (status != MagickFalse)
            status&=DrawDashPolygon(draw_info,primitive_info,image);
          break;
        }
      mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
      if ((mid > 1.0) &&
          ((draw_info->stroke.opacity != (Quantum) TransparentOpacity) ||
           (draw_info->stroke_pattern != (Image *) NULL)))
        {
          double
            x,
            y;

          MagickBooleanType
            closed_path;

          /*
            Draw strokes while respecting line cap/join attributes.
          */
          closed_path=primitive_info[0].closed_subpath;
          i=(ssize_t) primitive_info[0].coordinates;
          x=fabs(primitive_info[i-1].point.x-primitive_info[0].point.x);
          y=fabs(primitive_info[i-1].point.y-primitive_info[0].point.y);
          if ((x < MagickEpsilon) && (y < MagickEpsilon))
            closed_path=MagickTrue;
          if ((((draw_info->linecap == RoundCap) ||
                (closed_path != MagickFalse)) &&
               (draw_info->linejoin == RoundJoin)) ||
               (primitive_info[i].primitive != UndefinedPrimitive))
            {
              status&=DrawPolygonPrimitive(image,draw_info,primitive_info);
              break;
            }
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=(Quantum) TransparentOpacity;
          status&=DrawPolygonPrimitive(image,clone_info,primitive_info);
          clone_info=DestroyDrawInfo(clone_info);
          if (status != MagickFalse)
            status&=DrawStrokePolygon(image,draw_info,primitive_info);
          break;
        }
      status&=DrawPolygonPrimitive(image,draw_info,primitive_info);
      break;
    }
  }
  image_view=DestroyCacheView(image_view);
  if (draw_info->compliance == SVGCompliance)
    {
      status&=SetImageClipMask(image,(Image *) NULL);
      status&=SetImageMask(image,(Image *) NULL);
    }
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"  end draw-primitive");
  return(status != 0 ? MagickTrue : MagickFalse);
}