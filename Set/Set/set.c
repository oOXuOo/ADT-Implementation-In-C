/* 
 * filename: set.c
 * author: zhm
 * date: 2013-01-06
 */

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "set.h"

/* set_init */
void set_init(Set *set, int (*match)(const void *key1, const void *key2),
        void (*destroy)(void *data))
{
    /* init the list */
    list_init(set, destroy);
    set->match = match;
    return;
}
/* set_insert */
int set_insert(Set *set, const void *data)
{
    /* Do not allow the insertion of duplicates. */
    if( set_is_member(set, data) )
        return 1;

    return list_ins_next(set, list_tail(set), data); //调用链表的插入元素操作
}
/* set_remove */
int set_remove(Set *set, void **data)
{
    ListElmt *member, *prev; //注意prev，用于记录被删除元素前面的那一元素位置，为后续删除作好准备

    /* Find the member to remove */
    prev = NULL;

    for( member = list_head(set); member != NULL; member = list_next(member) )
    {
        if( set->match(list_data(member), *data) )
        {
            break;
        }
        prev = member;
    }

    /* Return if the member was not found */
    if( member == NULL )
        return -1;

    /* remove the member */
    return list_rem_next(set, prev, data);
}
/* set_union */
int set_union(Set *setu, const Set *set1, const Set *set2)
{
    void *data;
    ListElmt *member;
   /* initialize the set for the union */
    set_init( setu, set1->match, NULL );

    /* Insert the members of the first set */
    for( member = list_head(set1); member != NULL; member = list_next(member) )
    {
        data = list_data(member);
        if( list_ins_next(setu, list_tail(setu), data) != 0 )
        {
            set_destroy(setu);
            return -1;
        }
    }

    /* Insert the members of the second set. */
    for( member = list_head(set2); member != NULL; member = list_next(member) )
    {
        if( set_is_member(set1, list_data(member)) )
        {
            continue;
        }
        else
        {
            data = list_data(member);
            if( list_ins_next(setu, list_tail(setu), data) != 0 )
            {
                set_destroy(setu);
                return -1;
            }
        }
    }

    return 0;
}
/* set_difference */
int set_difference(Set *setd, const Set *set1, const Set *set2)
{
    ListElmt *member;
    void *data;

    /* Initialize the set for the difference */
    set_init(setd, set1->match, NULL);

    /* Insert the members from set1 not in set2 */
    for( member = list_head(set1); member != NULL; member = list_next(member) )
    {
        if( !set_is_member(set2, list_data(member)) )
        {
            data = list_data(member);
            if( list_ins_next(setd, list_tail(setd), data) != 0 )
            {
                set_destroy(setd);
                return -1;
            }
        }
    }

    return 0;
}
/* set_intersection */
int set_intersection(Set *seti, const Set *set1, const Set *set2)
{
    ListElmt *member;
    void *data;

    /* initialize the set for the intersection */
    set_init(seti, set1->match, NULL);

    /* Insert the members present in both sets */
    for( member = list_head(set1); member != NULL; member = list_next(member) )
    {
        if( set_is_member(set2, list_data(member)) )
        {
            data = list_data(member);
            if( list_ins_next(seti, list_tail(seti), data) != 0 )
            {
                set_destroy(seti);
                return -1;
            }
        }
    }
    return 0;
}
/* set_is_member */
int set_is_member(const Set *set, const void *data)
{
    ListElmt *member;

    for(member = list_head(set); member != NULL; member = list_next(member))
    {
        if( set->match(list_data(member), data) )
        {
            return 1;
        }
    }

    return 0;
}
/* set_is_subset */
int set_is_subset(const Set *set1, const Set *set2)
{
    ListElmt *member;

    if( set_size(set1) > set_size(set2) )
        return 0;

    for(member = list_head(set1); member != NULL; member = list_next(member))
    {
        if( !set_is_member(set2, list_data(member)) )
        {
            return 0;
        }
    }

    return 1; 
}
/* set_is_equal */
int set_is_equal( const Set *set1, const Set *set2 )
{
    if( set_size(set1) != set_size(set2) )
        return 0;

    return set_is_subset(set1, set2);
}