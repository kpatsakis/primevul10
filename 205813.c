bool Item_field::collect_item_field_processor(uchar *arg)
{
  DBUG_ENTER("Item_field::collect_item_field_processor");
  DBUG_PRINT("info", ("%s", field->field_name ? field->field_name : "noname"));
  List<Item_field> *item_list= (List<Item_field>*) arg;
  List_iterator<Item_field> item_list_it(*item_list);
  Item_field *curr_item;
  while ((curr_item= item_list_it++))
  {
    if (curr_item->eq(this, 1))
      DBUG_RETURN(FALSE); /* Already in the set. */
  }
  item_list->push_back(this);
  DBUG_RETURN(FALSE);
}