#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"

#include "../private/common.h"

static uint16_t values[256];

static void list_merge(struct list_head *head,
                       struct list_head *l1,
                       struct list_head *l2)
{
    struct list_head *tmp;
    while (1) {
        if (list_empty(l1)) {
            list_splice_tail(l2, head);
            return;
        } else if (list_empty(l2)) {
            list_splice_tail(l1, head);
            return;
        }

        if (container_of(l1->next, struct listitem, list)->i <=
            container_of(l2->next, struct listitem, list)->i) {
            tmp = l1->next;
        } else {
            tmp = l2->next;
        }
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    return;
}
static void list_msort(struct list_head *head, int n)
{
    if (list_empty(head) || list_is_singular(head))
        return;
    int i = 0;
    struct listitem *entry;
    struct list_head *node = NULL;
    struct list_head l1, l2;
    INIT_LIST_HEAD(&l1);
    INIT_LIST_HEAD(&l2);
    list_for_each (node, head) {
        i++;
        if (i >= n / 2)
            break;
    }
    list_cut_position(&l1, head, node);
    list_cut_position(&l2, head, head->prev);
    list_msort(&l1, n / 2);
    list_msort(&l2, n - n / 2);
    list_merge(head, &l1, &l2);
    return;
}
int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_msort(&testlist, 256);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}