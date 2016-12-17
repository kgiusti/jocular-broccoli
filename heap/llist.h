/*
 *    Copyright 2016, Kenneth A Giusti
 *
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may
 *    not use this file except in compliance with the License. You may obtain
 *    a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *    License for the specific language governing permissions and limitations
 *    under the License.
 */

#define DEFINE_LIST(type, name)       \
    typedef struct {                    \
        type *head, *tail;              \
    } name##_list_t;

#define LIST_LINKAGE(type, name)     \
    struct { \
    type *prev;          \
    type *next;          \
    } name;

INSERT_HEAD(l, x);
INSERT_TAIL(l, x);
REMOVE(l, x);
INSERT_BEFORE(z, x);
INSERT_AFTER(z, x);

#DEFINE_LIST(struct asshole, alist);

typedef struct {
    struct asshole *head, *tail;
} alist_list_t;

#define LIST_INIT(L) (L)->head = (L)->tail = NULL;

typedef struct dickhead {
    int a;
    LIST_LINKAGE(struct dickhead, alist);
    int b;
    LIST_LINKAGE(struct dickhead, blist);
} dickhead_t;

typedef struct dickhead {
    int a;
    struct { struct dickhead  *prev, *next } alist;
    int b;
    struct { struct dickhead  *prev, *next; } blist;
} dickhead_t;

for (a = d1; a != NULL; a = a->alist.next)
    ;

LINKAGE_INIT(L)  (L)->next = (L)->prev = NULL;


INSERT_HEAD(L, O, N)                             \
O->N##.next = L->head;
L->head = O;
if (!(L)->tail) (L)->tail = O;
O->N##.prev = NULL;

INSERT_TAIL(L, O, N)
{
    O->N##.prev = L->tail;
    L->tail = O;
    O->N##.next = NULL;
    if (!L->head) L->head = 0;
}


void insert_head(list *l1, obj *n)
{
    n-><listname>.next = l1->head;
    l1->head-><listname>.prev = n;
    l1->head = n;
    if (l1->tail == NULL) l1->tail = n;
    n->prev = NULL;
}
