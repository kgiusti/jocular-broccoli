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

#define DEFINE_LIST(T, N)       \
    typedef struct {            \
        T *head, *tail;         \
        int  count;             \
    } N##_list_t

#define LIST_INIT(L)            \
    do {                        \
        (L)->head = NULL;       \
        (L)->tail = NULL;       \
        (L)->count = 0;         \
    } while (0)

#define LIST_LINKAGE(T, F) T *prev##F, *next##F

#define LINK_INIT(N, F)         \
    do {                        \
        (N)->prev##F = NULL;    \
        (N)->next##F = NULL;    \
    } while (0)

#define LIST_REMOVE(L, N, F)                            \
    do {                                                \
        if ((N)->next##F)                               \
            (N)->next##F->prev##F = (N)->prev##F;       \
        else                                            \
            (L)->tail = (N)->prev##F;                   \
        if ((N)->prev##F)                               \
            (N)->prev##F->next##F = (N)->next##F;       \
        else                                            \
            (L)->head = (N)->next##F;                   \
       (N)->next##F = (N)->prev##F = NULL;              \
       (L)->count--;                                    \
    } while (0)

#define POP_HEAD(L, N, F)               \
    do {                                \
        N = (L)->head;                  \
        if (N) LIST_REMOVE(L, N, F);    \
    } while (0);

#define PUSH_TAIL(L, N, F)      \
do {                            \
    (N)->next##F = NULL;        \
    (N)->prev##F = (L)->tail;   \
    (L)->tail = (N);            \
    if ((N)->prev##F)                   \
        (N)->prev##F->next##F = (N);    \
    else                                \
        (L)->head = (N);        \
    (L)->count++;               \
 } while (0)

#define LIST_NEXT(N, F)   ((N)->next##F)
#define LIST_PREV(N, F)   ((N)->prev##F)


