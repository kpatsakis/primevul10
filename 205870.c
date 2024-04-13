void Item_field::fix_after_pullout(st_select_lex *new_parent, Item **ref)
{
  if (new_parent == get_depended_from())
    depended_from= NULL;
  if (context)
  {
    bool need_change= false;
    /*
      Suppose there are nested selects:

       select_id=1
         select_id=2
           select_id=3  <----+
             select_id=4    -+
               select_id=5 --+

      Suppose, pullout operation has moved anything that had select_id=4 or 5
      in to select_id=3.

      If this Item_field had a name resolution context pointing into select_lex
      with id=4 or id=5, it needs a new name resolution context.

      However, it could also be that this object is a part of outer reference:
      Item_ref(Item_field(field in select with select_id=1))).
      - The Item_ref object has a context with select_id=5, and so needs a new
        name resolution context.
      - The Item_field object has a context with select_id=1, and doesn't need
        a new name resolution context.

      So, the following loop walks from Item_field's current context upwards.
      If we find that the select we've been pulled out to is up there, we
      create the new name resolution context. Otherwise, we don't.
    */
    for (Name_resolution_context *ct= context; ct; ct= ct->outer_context)
    {
      if (new_parent == ct->select_lex)
      {
        need_change= true;
        break;
      }
    }
    if (!need_change)
      return;

    Name_resolution_context *ctx= new Name_resolution_context();
    if (context->select_lex == new_parent)
    {
      /*
        This field was pushed in then pulled out
        (for example left part of IN)
      */
      ctx->outer_context= context->outer_context;
    }
    else if (context->outer_context)
    {
      /* just pull to the upper context */
      ctx->outer_context= context->outer_context->outer_context;
    }
    else
    {
      /* No upper context (merging Derived/VIEW where context chain ends) */
      ctx->outer_context= NULL;
    }
    ctx->table_list= context->first_name_resolution_table;
    ctx->select_lex= new_parent;
    if (context->select_lex == NULL)
      ctx->select_lex= NULL;
    ctx->first_name_resolution_table= context->first_name_resolution_table;
    ctx->last_name_resolution_table=  context->last_name_resolution_table;
    ctx->error_processor=             context->error_processor;
    ctx->error_processor_data=        context->error_processor_data;
    ctx->resolve_in_select_list=      context->resolve_in_select_list;
    ctx->security_ctx=                context->security_ctx;
    this->context=ctx;
  }
}