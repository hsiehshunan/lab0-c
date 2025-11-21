#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)  return NULL;//Check if malloc is successful or not.
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
    if(!s) return false; // nothing to add
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
    if(!head) return NULL; // nothing to remove
    element_t *front = list_first_entry(head, element_t, list);
    list_del(&front->list);

    if(bufsize > 0){ // make sure bifsize > 0 to avoid bufsize - 1 < 0
        strncpy(sp, front->value, bufsize-1);
        sp[bufsize-1] = '\0';
    }

    return front;

}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if(!head) return NULL; // nothing to remove
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
    while( front->next != rear && front != rear)
    {
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
    if(!head || list_empty(head) ) return false;  // to handle NULL, empty node
    if(list_is_singular(head)) {return true;} // to handle one single node.

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
    node1 = node1->next, node2 = node1->next)
    {
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

    while(count--)
    {
        struct list_head tmp;
        INIT_LIST_HEAD(&tmp);    //create and initialize empty list

         /* Move k nodes after group_prev into tmp, reversing their order */
        for(int i = 0; i < k; i++)
        {
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
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if(!head || list_empty(head)) return 0;
    if(list_is_singular(head)) return 1;
    struct list_head *prev1 = head->prev, *prev2 = head->prev;
    
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
