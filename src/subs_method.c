#include "subs_method.h"
#include <stdio.h>

#define TRUE    1
#define FALSE   0

/**
 * The implementations of the algorithms are thoroughly described in
 * the report.
 */

/* LEAST RECENTLY USED */
void lru_add(PageList* page_list, PageEntry* page_entry)
{
    add_page_list_last(page_entry, page_list);
}

Page* lru_subs(PageList* page_list)
{
    PageEntry* page_entry = remove_page_list_first(page_list);
    Page* page = get_page(page_entry);

    free_page_entry(page_entry, FALSE);

    return page;
}

void lru_update(int index, char mode, unsigned int time, PageList* page_list)
{
    PageEntry* page_entry = remove_page_list_index(index, page_list);

    // Page update
    set_last_ref(page_entry, time);
    set_mflag(page_entry, mode);

    add_page_list_last(page_entry, page_list);
}

/* FIRST IN FIRST OUT WITH SECOND CHANCE (FIFO SC) */
void sc_add(PageList* page_list, PageEntry* page_entry)
{
    add_page_list_last(page_entry, page_list);
}

Page* sc_subs(PageList* page_list)
{
    PageEntry* page_entry;
    Page* page;

    page_entry = sc_procedure(page_list);
    page = get_page(page_entry);

    free_page_entry(page_entry, FALSE);

    return page;
}

void sc_update(int index, char mode, PageList* page_list)
{
    PageEntry* page_entry = search_page_list(index, page_list);

    set_rflag(page_entry, TRUE);
    set_mflag(page_entry, mode);
}

static int cycle_update(PageList* page_list, unsigned int subs_cycle)
{
    static unsigned int cycle_count = 0;
    if (++cycle_count > subs_cycle)
    {
        off_rflag_all(page_list);
        cycle_count = 0;
        return TRUE;
    }

    return FALSE;
}

/* NOT RECENTLY USED */
int flag = TRUE; // Avoids duplicate execution of cycle update in case of substitution.

void nru_add(PageList* page_list, PageEntry* page_entry)
{
    if (flag == TRUE)
    {
        if (cycle_update(page_list, get_entry_max(page_list)) == TRUE)
            ord_page_list(page_list, cmp_nru);
    }
    add_page_list_ord(page_entry, page_list, cmp_nru);

    flag = TRUE;
}

Page* nru_subs(PageList* page_list)
{
    PageEntry* page_entry;
    Page* page;
    
    flag = FALSE;

    if (cycle_update(page_list, get_entry_max(page_list)) == TRUE)
        ord_page_list(page_list, cmp_nru);

    page_entry = remove_page_list_first(page_list);
    page = get_page(page_entry);

    free_page_entry(page_entry, FALSE);

    return page;
}

void nru_update(int index, char mode, unsigned int time, PageList* page_list)
{
    PageEntry* page_entry;

    page_entry = remove_page_list_index(index, page_list);
    set_last_ref(page_entry, time);
    set_mflag(page_entry, mode);
    set_rflag(page_entry, TRUE);

    cycle_update(page_list, get_entry_max(page_list));
    
    add_page_list_ord(page_entry, page_list, cmp_nru);
}

/* OPTIMAL ALGORITHM */
void optimal_add(PageList* page_list, PageEntry* page_entry)
{
    add_page_list_ord(page_entry, page_list, cmp_optimal);
}

Page* optimal_subs(PageList* page_list)
{
    PageEntry* page_entry = remove_page_list_first(page_list);
    Page* page = get_page(page_entry);

    free_page_entry(page_entry, FALSE);

    return page;
}

void optimal_update(int index, char mode, unsigned int time, unsigned int next_ref, PageList* page_list)
{
    PageEntry* page_entry = remove_page_list_index(index, page_list);

    // Page update
    /*
        Comment set_last_ref for a gain in performance, as last_ref
        is not necessary for the optimal algorithm to operate.
    */
    // set_last_ref(page_entry, time); 
    set_next_ref(page_entry, next_ref);
    set_mflag(page_entry, mode);

    add_page_list_ord(page_entry, page_list, cmp_optimal);
}
