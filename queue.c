#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)  return NULL;//  malloc failed then return NULL.
    INIT_LIST_HEAD(q); // Initialize list head
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {
    if(!head) return;// nothing to free. 

    element_t *pos, *next;
    list_for_each_entry_safe(pos, next, head, list){
        list_del(&pos->list);
        q_release_element(pos);
    }
    test_free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if(!head) return false;
    if(!s) return false; 

    element_t *node = malloc(sizeof(element_t));
    if (!node) return false;
    node->value = strdup(s);
    INIT_LIST_HEAD(&node->list);
    list_add(&node->list, head);
    
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if(!head) return false; // nothing to add
    if(!s) return false; // nothing to add
    element_t *node = malloc(sizeof(*node));
    if(!node) return false; //memory allocation failed for element_t
    node->value = strdup(s);
    INIT_LIST_HEAD(&node->list);
    list_add_tail(&node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if(!head || (list_empty(head))) return NULL; // nothing to remove
    
    element_t *front = list_first_entry(head, element_t, list);
    list_del(&front->list);

    if(bufsize > 0){ // make sure bufsize > 0 to avoid bufsize - 1 < 0
        strncpy(sp, front->value, bufsize-1);
        sp[bufsize-1] = '\0';
    }

    return front;

}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if(!head || (list_empty(head))) return NULL; // nothing to remove
    
    element_t *tail = list_last_entry(head, element_t, list);
    list_del(&tail->list);
    
    if(bufsize > 0 ){ // make sure bifsize > 0 to avoid bufsize - 1 < 0
        strncpy(sp, tail->value, bufsize-1);
        sp[bufsize-1] = '\0'; 
    }
    return tail;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{   // to handle NULL, empty node
    if(!head || list_empty(head) ) return false;  
    
    // to handle one single node.
    if(list_is_singular(head)) { 
        struct list_head *node_list = head->next;
        element_t *node = list_entry(node_list, element_t,list);
        list_del(node_list);
        q_release_element(node);
        return true;
    }

    //to handle two or more nodes. 
    struct list_head *front = head->next ;
    struct list_head *rear = head->prev;
    while( front->next != rear && front != rear){
        front = front ->next; rear = rear->prev;
    }
    element_t * node = list_entry(rear, element_t, list);
    list_del(rear);  // when loop ends, 'rear' points to the ⌊n/2⌋-th node
    q_release_element(node);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if(!head || list_empty(head) ) return false;  //  handle NULL, empty node
    if(list_is_singular(head)) return true; //  handle one single node.

    struct list_head *curr = head->next;

    while( curr!= head && curr->next != head){
        element_t *e_curr = list_entry(curr,element_t,list);
        element_t *e_next = list_entry(curr->next,element_t,list);
        if(strcmp(e_curr->value, e_next->value) == 0){
            char *dup_val = e_curr->value;
            do{
                struct list_head *to_del = curr;
                element_t *e_del = list_entry(to_del, element_t, list);
                curr = curr->next;
                list_del(to_del);
                q_release_element(e_del);
                if(curr == head)break;
            }while(strcmp(list_entry(curr, element_t, list)->value,dup_val) == 0) ;
        }else{
            curr = curr->next;
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;

}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if(!head || list_empty(head)) return NULL;
    if(list_is_singular(head)) return ;
    struct list_head *node1;
    struct list_head *node2;
    struct list_head *temp;
    for(node1 = head->next, node2 = head->next->next; node1 != head && node2 != head;
    node1 = node1->next, node2 = node1->next){
        temp->next = node1->next;
        temp->prev = node1->prev;
        
        node1->next = node2->next;  
        node2->next->prev = node1;  
        
        node1->prev = node2; 
        node2->next = node1;  
        
        temp->prev->next = node2;
        node2->prev = temp->prev;
    }


    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {
    if(!head || list_empty(head) || list_is_singular(head)) return;
    
    struct list_head new_head;                   // create and initialize a new head.
    INIT_LIST_HEAD(&new_head);

    while(!list_empty(head)){                   // check wheter the old list's head is empty
        struct list_head *tail = head->prev;    // take the last node of the old list
        list_move_tail(tail,&new_head);          // add the last node from old list to tail of new list.  
    }
    list_splice(&new_head, head);               // list_splice(reversed list, empty list) is result
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if(!head || list_empty(head) || list_is_singular(head)) return;
    if(k < 2) return;

    int count = q_size(head)/k ; //Count the numnber of group that we need to reverse.
    if(count == 0) return;
    struct list_head *group_list ;
    group_list = head;

    while(count--){
        struct list_head tmp;
        INIT_LIST_HEAD(&tmp);    //create and initialize empty list

         /* Move k nodes after group_prev into tmp, reversing their order */
        for(int i = 0; i < k; i++){
            struct list_head *node = group_list->next;  // always the first node in the remaining group
            list_move(node, &tmp);  // append the node to the new list c, b, a, say k = 3
        }
        /* Now tmp contains the k nodes in reversed order.
         * Splice them back after group_prev.
         */
        list_splice_init(&tmp,group_list);
        
        /* Advance group_prev to the end of this reversed group
         * (k steps forward).
         */
        for(int i = 0 ; i < k; i++){
            group_list = group_list->next;
        }
    }

    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) 
{
    struct list_head *list;
    struct list_head *part[65];
    int level;

    /* 0 或 1 個節點，不用排 */
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* 取出原本 list 的第一個與最後一個節點 */
    struct list_head *first = head->next;
    struct list_head *last  = head->prev;

    last->next = NULL; // turn doubly linked list to singly linked list
    list = first;

    /* initialize head to empty list   */
    INIT_LIST_HEAD(head);

    memset(part, 0, sizeof(part)); // initialize part to NULL

    while (list) {
        struct list_head *curr = list;
        struct list_head *next = list->next;

        curr->next = NULL;
        curr->prev = NULL;

        list = next;


        for (level = 0; part[level]; level++) {
            curr = merge_two_sorted_list(descend, part[level], curr);
            part[level] = NULL;
        }
        part[level] = curr;
    }
    list = NULL;
    for (level = 0;
         level < (int) (sizeof(part) / sizeof(part[0]));
         level++) {
        if (!part[level])
            continue;
        list = merge_two_sorted_list(descend, part[level], list);
    }

    /* list now is 
     *   n1 → n2 → ... → nk → NULL
     *
     * turn back to doubly linked list
     */
    while (list) {
        struct list_head *next = list->next;
        list_add_tail(list, head);
        list = next;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if(!head || list_empty(head)) return 0;
    if(list_is_singular(head)) return 1;
    
    
    
    struct list_head *node, *safe;
    element_t *e;
    char *max_value;
    e = list_entry(head->next, element_t, list);
    max_value = e->value;
    // check the max greater than all, if yes, move to next node and assign it to the max and repeat the behavior to the end. 
    list_for_each_safe(node, safe,head){ 
        e = list_entry(node, element_t, list);

        if(strcmp(e->value, max_value) < 0){
            list_del(node);
            q_release_element(node);
        }else{
            max_value = e->value;
        }
    }
    

    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 1;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if(!head || list_empty(head)) return 0;
    if(list_is_singular(head)) return 1;

    struct list_head *node, *safe;
    element_t *e;
    char *min_value;
    e = list_entry(head->next, element_t, list);
    min_value = e->value;
    // check the min less than all, if yes, move to next node and assign it to the min and repeat the behavior to the end. 
    list_for_each_safe(node, safe,head){ 
        e = list_entry(node, element_t, list);

        if(strcmp(e->value, min_value) > 0){
            list_del(node);
            q_release_element(node);
        }else{
            min_value = e->value;
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 1;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge_two(struct list_head *l1, struct list_head *l2, bool descend)
{
    if(!l1 || !l2) return 0;

    /*If one list is empty, move the other to l1*/
    if(list_empty(l1)){
        if(!list_empty(l2)){
            list_splice_init(l2, l1);  // move l2 to l1
        }
        return q_size(l1);
    }
    if(list_empty(l2)){
        return q_size(l1);
    }
    struct list_head merged;
    INIT_LIST_HEAD(&merged);
    
    struct list_head *n1 = l1->next;
    struct list_head *n2 = l2->next;

    while( n1 != l1 && n2 != l2){
        element_t *e1 = list_entry(n1, element_t, list);
        element_t *e2 = list_entry(n2, element_t, list);
        int cmp = strcmp(e1->value, e2->value);
        
        bool grab = descend ? (cmp>=0) : (cmp <= 0);

        if(grab){
            struct list_head *next = n1->next;
            list_move_tail(n1, &merged);
            n1 = next;
        }else{
            struct list_head *next = n2->next;
            list_move_tail(n2, &merged);
            n2 = next ; 
        }

    }
// Move remaining nodes from list to tmp list;
    while(n1 != l1){
        struct list_head *next = n1->next;
        list_move_tail(n1, &merged);
        n1 = next;
    }

    while (n2 != l2){
        struct list_head *next = n2->next;
        list_move_tail(n2, &merged);
        n2 = next;
    }
    INIT_LIST_HEAD(l1);
    list_splice_init(&merged, l1);
    INIT_LIST_HEAD(l2);
    
    return q_size(l1);

}

int q_merge(struct list_head *head, bool descend)
{
    descend = (descend != 0);  // if ascend then 0 if descend 1.
    if(!head || list_empty(head)) return 0;
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if(list_is_singular(head)){
        queue_contex_t *ctx = list_entry(head, queue_contex_t, chain);
        return ctx->size;
    }
    /* More than one context:
     * use the first one as accumulator (ctx0)
     */
    queue_contex_t *ctx0 = list_entry(head->next, queue_contex_t, chain);

    struct list_head *acc = ctx0->q;

    int total = ctx0->size ; 
    
    struct list_head *pos;

    for(pos = ctx0->chain.next ; pos != head; pos = pos->next){
        queue_contex_t *ctx = list_entry(pos, queue_contex_t, chain);


        /* Skip if this context's queue is NULL or empty */
        if (!ctx->q || list_empty(ctx->q))
            continue;
        total = q_merge_two(acc, ctx->q, descend);

                /* After merging:
         *  - acc holds all elements from previous queues + ctx->q
         *  - ctx->q is empty
         * Update sizes accordingly.
         */
        ctx0->size = total;
        ctx->size  = 0;
    }

    return total;
}

// a helper function for merge_two_sorted_list for q_sort

static struct list_head *merge_two_sorted_list(struct list_head *a, struct list_head *b, bool descend)
{
    if(!a) return b;
    if(!b) return a;

    struct list_head *head;
    struct list_head **tail = &head;

    while(1 && b){
        element_t *ea = list_entry(a, element_t, list);
        element_t *eb = list_entry(b, element_t, list);
        int cmp = strcmp(ea->value, eb->value);

        bool take_a = descend ? (cmp>=0):(cmp<0);

        if(take_a){
            struct list_head *next = a->next;
            a->next = NULL;
            *tail = a;
            tail = &a->next;
            a = next;
        }else{
            struct list_head *next = b->next;
            b->next = NULL;
            *tail = b;
            tail = &b->next;
            b = next;
        }
    }   
    *tail = a ? a : b;

    return head;
}







