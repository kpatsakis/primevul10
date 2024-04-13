XML_SetCommentHandler(XML_Parser parser,
                      XML_CommentHandler handler)
{
  if (parser != NULL)
    commentHandler = handler;
}