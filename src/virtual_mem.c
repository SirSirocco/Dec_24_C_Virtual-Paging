#include "virtual_mem.h"
#include <stdio.h>
#include <stdlib.h>

#define error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define TRUE        1
#define FALSE       0

// ## STRUCTS ##
struct node
{
    Page*       page;
    PageEntry*  next;   
};

struct flag_table
{
    int referenced;             // Indicates the page was referenced.
    int modified;               // Indicates the page was modified (becomes 'dirty').
};

struct page 
{
    unsigned int    index;      // Page's index
    unsigned int    last_ref;   // Last time in which the page was referenced.
    unsigned int    next_ref;   // Next line of code in which the page will be referenced.
    FlagTable       flag;       // All flags the page may need.
};

struct page_list
{
    unsigned int    entry_num;  // Current number of entries.
    unsigned int    entry_max;  // Max number of entries.
    PageEntry*      first;      // First entry.
    PageEntry*      last;       // Last entry.
};

// ## PAGE ##
Page* create_page(unsigned int index, unsigned int last_ref, unsigned int next_ref, int fr, int fm)
{
    Page* page = (Page*)malloc(sizeof(Page));

    if (page == NULL) // Unable to allocate memory
        error("create_page");

    page->index = index;
    page->last_ref = last_ref;
    page->next_ref = next_ref;
    page->flag.referenced = fr;
    page->flag.modified = fm;

    return page;
}

void print_page(Page* page)
{
    if (page == NULL)
        return;

    printf("Index:          %X\n", page->index);
    printf("Last reference: %u\n", page->last_ref);
    printf("Next reference: %u\n", page->next_ref);
    printf("Flag R:         %d\n", page->flag.referenced);
    printf("Flag M:         %d\n", page->flag.modified);
}

void free_page(Page* page)
{
    free(page);
}

int check_dirty_page(Page* page)
{
    return page->flag.modified;
}

int get_index(Page* page)
{
    if (page == NULL)
        return -1;
    return page->index;
}

// ## PAGE ENTRY ##
PageEntry* create_node(Page* page, PageEntry* next)
{
    PageEntry* node = (PageEntry*)malloc(sizeof(PageEntry));

    if (node == NULL) // Unable to allocate memory
        error("create_node");

    node->page = page;
    node->next = next;

    return node;
}

PageEntry* create_page_entry(unsigned int index, unsigned int last_ref, unsigned int next_ref, int fr, int fm, PageEntry* next)
{
    Page* page = create_page(index, last_ref, next_ref, fr, fm);
    return create_node(page, next);
}

void print_page_entry(PageEntry* page_entry)
{
    if (page_entry == NULL) // NULL page entry
        return;
    
    print_page(page_entry->page);
    printf("Current:    %p\n", page_entry);
    printf("Next:       %p\n", page_entry->next);
}

void free_page_entry(PageEntry* page_entry, int free_pg)
{
    if (free_pg == TRUE)
        free_page(page_entry->page);

    free(page_entry);
}

Page* get_page(PageEntry* page_entry)
{
    if (page_entry == NULL)
        return NULL;
    return page_entry->page;
}

void set_last_ref(PageEntry* page_entry, unsigned int time)
{
    if (page_entry == NULL)
        return;
    
    page_entry->page->last_ref = time;
}

void set_next_ref(PageEntry* page_entry, unsigned int next_ref)
{
    if (page_entry == NULL)
        return;
    
    page_entry->page->next_ref = next_ref;
}

void set_rflag(PageEntry* page_entry, int value)
{
    if (page_entry == NULL)
        return;

    page_entry->page->flag.referenced = value;
}

void set_mflag(PageEntry* page_entry, char mode)
{
    if (page_entry == NULL)
        return;
    
    if (mode == 'W')
        page_entry->page->flag.modified = TRUE;
}

// ## PAGE LIST ##
PageList* create_page_list(unsigned int entry_max)
{
    PageList* page_list = (PageList*)malloc(sizeof(PageList));

    if (page_list == NULL) // Unable to allocate memory
        error("create_page_list");
    
    // Sets max number of elements
    page_list->entry_max = entry_max;

    // List begins empty
    page_list->entry_num = 0;
    page_list->first = page_list->last = NULL;

    return page_list;
}

void print_page_list(PageList* page_list)
{
    PageEntry* aux = page_list->first;

    printf("-> Entry count:    %u\n", page_list->entry_num);
    printf("-> Entry max:      %u\n", page_list->entry_max);
    printf("-> First element:  %p\n", page_list->first);
    printf("-> Last element:   %p\n", page_list->last);
    printf("# Elements:\n\n");
    while (aux != NULL)
    {
        print_page_entry(aux);
        printf("\n");
        aux = aux->next;
    }
}

void free_page_list(PageList* page_list, int free_pg)
{
    PageEntry* aux;

    while (page_list->first != NULL)
    {
        aux = page_list->first;
        page_list->first = page_list->first->next;
        free_page_entry(aux, free_pg);
    }

    free(page_list);
}

unsigned int get_entry_max(PageList* page_list)
{
    if (page_list == NULL)
        return 0;

    return page_list->entry_max;
}

int has_room(PageList* page_list)
{
    return (page_list->entry_num < page_list->entry_max) ? TRUE : FALSE;
}

int check_page_in_list(unsigned int index, PageList* page_list)
{
    return (search_page_list(index, page_list) == NULL) ? FALSE : TRUE;
}

void add_page_list_first(PageEntry* page_entry, PageList* page_list)
{
    page_list->entry_num++;

    page_entry->next = page_list->first;

    if (page_list->first == NULL)       // Empty list
        page_list->last = page_entry;   // First will also be last

    page_list->first = page_entry;
}

void add_page_list_last(PageEntry* page_entry, PageList* page_list)
{
    page_list->entry_num++;

    page_entry->next = NULL;                // Last element has no next

    if (page_list->last == NULL)            // Empty list
        page_list->first = page_entry;      // First will also be last
    else
        page_list->last->next = page_entry; // Links to previous last

    page_list->last = page_entry;
}

void add_page_list_ord(PageEntry* page_entry, PageList* page_list, int(*cmp)(Page* p1, Page* p2))
{
    PageEntry* aux = page_list->first;
    page_list->entry_num++;

    if (aux == NULL) // Empty list
    {
        page_entry->next = NULL;
        page_list->first = page_list->last = page_entry; // First will also be last
        return;
    }

    if (cmp(page_entry->page, aux->page) <= 0) // Add before first
    {
        page_entry->next = aux;
        page_list->first = page_entry;
        return;
    }

    while (aux->next != NULL && cmp(page_entry->page, aux->next->page) > 0) // Add after first
        aux = aux->next;

    page_entry->next = aux->next;
    aux->next = page_entry;

    if (page_entry->next == NULL)
        page_list->last = page_entry;
}

PageEntry* remove_page_list_first(PageList* page_list)
{
    PageEntry* aux;

    aux = page_list->first;

    if (aux == NULL) // Empty list
        return aux;

    page_list->entry_num--;

    if (aux == page_list->last) // Last element
        page_list->first = page_list->last = NULL;
    else
        page_list->first = page_list->first->next;

    aux->next = NULL; // Unlinks

    return aux;
}

PageEntry* remove_page_list_last(PageList* page_list)
{
   PageEntry *aux1 = page_list->last, *aux2;

    if (aux1 == NULL) // Empty list
        return aux1;

    page_list->entry_num--;

    if (aux1 == page_list->first) // Last element
        page_list->first = page_list->last = NULL;
    else
    {
        aux2 = page_list->first;

        // Finds previous of last element
        while (aux2->next != page_list->last)
            aux2 = aux2->next;
        
        page_list->last = aux2;         // Previous is new last
        page_list->last->next = NULL;   // New last has no next
    }

    aux1->next = NULL; // Unlinks

    return aux1;
}

PageEntry* remove_page_list_index(unsigned int index, PageList* page_list)
{
    PageEntry *aux1, *aux2;
    
    if (page_list == NULL)
        return NULL;
    
    aux1 = page_list->first;

    if (aux1 == NULL) // Empty list
        return NULL;

    if (aux1->page->index == index) // Removes first element
        return remove_page_list_first(page_list);

    // Tries to find it after the first
    while (aux1->next != NULL && aux1->next->page->index != index)
        aux1 = aux1->next;

    if (aux1->next == NULL) // Not found
        return NULL;

    page_list->entry_num--;

    aux2 = aux1->next;
    aux1->next = aux2->next;

    // Removes last
    if (aux2 == page_list->last)
        page_list->last = aux1; // Updates last

    aux2->next = NULL; // Unlinks
    
    return aux2;
}

PageEntry* search_page_list(unsigned int index, PageList* page_list)
{
    PageEntry* aux;

    aux = page_list->first;

    while (aux != NULL && aux->page->index != index)
        aux = aux->next;

    return aux;
}

void ord_page_list(PageList* page_list, int cmp(Page* p1, Page* p2))
{
    PageEntry *progressor, *lesser;

    // Empty list or list with only one element
    if (page_list == NULL || page_list->first == NULL || page_list->first == page_list->last)
        return;

    progressor = page_list->first;

    while (progressor != NULL)
    {
        lesser = progressor;
        
        while (progressor != NULL)
        {
            if (cmp(progressor->page, lesser->page) < 0)
                lesser = progressor;
            
            progressor = progressor->next;
        }

        progressor = remove_page_list_index(lesser->page->index, page_list);
        add_page_list_ord(progressor, page_list, cmp);
        progressor = progressor->next;
    }
}

// ### COMPARE FUNCTIONS ###
int cmp_index(Page* p1, Page* p2)
{
    if (p1->index < p2->index)
        return -1;
    
    else if (p1->index == p2->index)
        return 0;
    
    else
        return 1;
}

// ## FUNCTIONS FOR SUBS METHODS ##

// ### LRU ###
int cmp_lru(Page* p1, Page* p2)
{
    return p1->last_ref - p2->last_ref;
}

// ### FIFO SC ###
PageEntry* sc_procedure(PageList* page_list)
{
    PageEntry* aux;

    if (page_list == NULL) // NULL list
        return NULL;

    aux = page_list->first;

    if (aux == NULL) // Empty list
        return NULL;
    
    while (aux != NULL)
    {
        if (aux->page->flag.referenced == FALSE)
            return remove_page_list_index(aux->page->index, page_list);
        
        aux->page->flag.referenced = FALSE;
        aux = aux->next;
    }

    // Emulates circular linked list
    return remove_page_list_first(page_list);
}

// ### NRU ###
/**
 * Defines the NRU priority. The greater the priority,
 * the more a page should remain in memory.
 */
static int nru_priority(Page* page)
{
    if (page == NULL)
        return -1;

    return page->flag.modified + 2 * page->flag.referenced;
}

int cmp_nru(Page* p1, Page* p2)
{
    int delta =  nru_priority(p1) - nru_priority(p2);

    if (delta)
        return delta;

    else
        return cmp_lru(p1, p2);
}

void off_rflag_all(PageList* page_list)
{
    PageEntry* entry;

    if (page_list == NULL || page_list->first == NULL)
        return;

    entry = page_list->first;

    while (entry != NULL)
    {
        set_rflag(entry, FALSE);
        entry = entry->next;
    }
}

// ### OPTIMAL ###
int cmp_optimal(Page* p1, Page* p2)
{
    if (p1->next_ref > p2->next_ref)
        return -1;
    else if (p1->next_ref == p2->next_ref)
        return p1->flag.modified - p2->flag.modified;
    return 1;
}
