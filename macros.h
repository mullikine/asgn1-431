/* https://stackoverflow.com/questions/400951/does-c-have-a-foreach-loop-construct?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */

#define FOR_EACH(item, list) \
    for (ll_node *(item) = (list); (item); (item) = (item)->next)
