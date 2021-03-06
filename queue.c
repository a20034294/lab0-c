#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "natsort/strnatcmp.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));

    if (!q) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    // cppcheck-suppress variableScope
    list_ele_t *p, *tmp;

    if (!q)
        return;

    p = q->head;
    while (p) {
        free(p->value);
        tmp = p;
        p = p->next;
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    char *tmp_s;
    int len = strlen(s);

    if (!q)
        return false;

    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    tmp_s = malloc(len + 1);
    if (!tmp_s) {
        free(newh);
        return false;
    }
    strncpy(tmp_s, s, len + 1);

    newh->value = tmp_s;
    newh->next = q->head;
    newh->len = len;
    hash(newh);
    q->head = newh;
    if (!q->size)
        q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newt;
    char *tmp_s;
    int len = strlen(s);

    if (!q)
        return false;

    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;

    tmp_s = malloc(len + 1);
    if (!tmp_s) {
        free(newt);
        return false;
    }
    strncpy(tmp_s, s, len + 1);

    newt->value = tmp_s;
    newt->next = NULL;
    newt->len = len;
    hash(newt);
    if (!q->size)
        q->head = q->tail = newt;
    else
        q->tail->next = newt;
    q->tail = newt;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *head;

    if (!q || !q->head)
        return false;

    head = q->head;
    if (sp) {
        strncpy(sp, head->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    q->head = q->head->next;
    q->size--;
    free(head->value);
    free(head);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    list_ele_t *p, *prev, *next;

    if (!q || q->size <= 1)
        return;

    p = q->head;
    prev = NULL;
    while (p) {
        next = p->next;
        p->next = prev;
        prev = p;
        p = next;
    }

    // swap
    p = q->head;
    q->head = q->tail;
    q->tail = p;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    list_ele_t *p;

    if (!q || q->size <= 1)
        return;
    q->head = merge_sort(q->head, q->size);
    for (p = q->head; p; p = p->next)
        q->tail = p;
}
/*
 * Merge sort
 * @param head is the list head to sort
 * @param size is the size of list
 */
list_ele_t *merge_sort(list_ele_t *head, int size)
{
    int mid = size >> 1;
    int i;
    list_ele_t *pmid, *next;

    if (size == 1)
        return head;
    pmid = head;
    for (i = 0; i < mid; i++) {
        next = pmid->next;
        if (i == mid - 1)
            pmid->next = NULL;
        pmid = next;
    }

    head = merge_sort(head, mid);
    pmid = merge_sort(pmid, size - mid);
    return merge(head, pmid);
}

/*
 * Merge the two list to one
 * @listhead a
 * @listhead b
 * @return newhead
 */
list_ele_t *merge(list_ele_t *a, list_ele_t *b)
{
    list_ele_t *newh = NULL, **indirect = &newh;

    while (a || b) {
        if (!b ||
            (a && a->hash != b->hash && strnatcmp(a->value, b->value) != 1)) {
            *indirect = a;
            a = a->next;
        } else {
            *indirect = b;
            b = b->next;
        }
        indirect = &(*indirect)->next;
    }
    *indirect = NULL;
    return newh;
}

/*
 * Get the string hash
 * @char* s string
 */
inline void hash(list_ele_t *e)
{
    int i;
    e->hash = 0;
    for (i = 0; i < e->len; ++i) {
        e->hash *= 10000007;
        e->hash += e->value[i];
    }
}
