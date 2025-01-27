#include "virtual_mem.h"
#include <stdio.h>
#include <stdlib.h>
#define TRUE    1
#define FALSE   0

// create_page:
void test_create_page(void)
{
    Page* page = create_page(0xFFFFFFu, 1u, 200u, 1, 0);
    print_page(page);
    free_page(page);
}

// create_node:
void test_create_node(void)
{
    Page *page0 = create_page(0xFFFFFFu, 1u, 200u, 1, 0),
         *page1 = create_page(0xFFFFFFu, 2u, 400u, 0, 1);

    PageEntry *pg_entry0 = create_node(page0, NULL),
              *pg_entry1 = create_node(page1, pg_entry0);

    // Case 0:
    print_page_entry(pg_entry0);
    printf("\n");

    // Case 1:
    print_page_entry(pg_entry1);
    printf("\n");

    free_page_entry(pg_entry0, 1);
    free_page_entry(pg_entry1, 0);

    // Teste free_page_entry
    print_page(page0); // Error expected
    printf("\n");
    print_page(page1);
    printf("\n");
    free_page(page1);
}

// page_list:
void test_create_page_list_empty(void)
{
    PageList* list = create_page_list(100);
    print_page_list(list);
    free_page_list(list, TRUE);

    // print_page_list(list);
    // Exp.: Error
    // Act.: Error
}

void test_add_page_entry_first(void)
{
    PageList* list = create_page_list(100);
    PageEntry *page_entry0 = create_page_entry(100, 100, 100, 1, 1, NULL),
              *page_entry1 = create_page_entry(200, 200, 200, 1, 1, NULL);

    print_page_list(list);
    printf("\n");

    // 1 Element
    add_page_list_first(page_entry0, list);
    print_page_list(list);
    printf("\n");

    // 2 Elements
    add_page_list_first(page_entry1, list);
    print_page_list(list);
    printf("\n");

    free_page_list(list, TRUE);
}

void test_add_page_entry_last(void)
{
    PageList* list = create_page_list(100);
    PageEntry *page_entry0 = create_page_entry(100, 100, 100, 1, 1, NULL),
              *page_entry1 = create_page_entry(200, 200, 200, 1, 1, NULL);

    print_page_list(list);
    printf("\n");

    // 1 Element
    add_page_list_last(page_entry0, list);
    print_page_list(list);
    printf("\n");

    // 2 Elements
    add_page_list_last(page_entry1, list);
    print_page_list(list);
    printf("\n");

    free_page_list(list, TRUE);
}

void test_free_page_list(int free_pg)
{
    PageList* list = create_page_list(100);
    Page* page = create_page(100, 100, 100, 1, 1);
    PageEntry* page_entry = create_node(page, NULL);

    print_page_list(list);
    printf("\n");

    printf("!!! PAGE ENTRY BEFORE FREE:\n");
    print_page_entry(page_entry);
    printf("\n");

    printf("!!! PAGE BEFORE FREE:\n");
    print_page(page);
    printf("\n");

    free_page_list(list, free_pg);

    printf("!!! PAGE ENTRY AFTER FREE:\n");
    print_page_entry(page_entry);
    printf("\n");

    printf("!!! PAGE AFTER FREE:\n");
    print_page(page);
    printf("\n");

    free_page(page);
}

#define NUM 3

void test_add_page_list_ord(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 };
    PageList* page_list = create_page_list(200);

    for (int i = 0; i < NUM; i++)
    {
        add_page_list_ord(create_page_entry(addr[i], 1, 1, 1, 1, NULL),
                            page_list, cmp_index);
        print_page_list(page_list);
    }

    free_page_list(page_list, TRUE);
}

void test_search_page_entry(void)
{
    unsigned int    addr[NUM] = { 10, 9, 1 },
                    to_find[NUM] = { 10, 9, 1 },
                    not_to_find[NUM] = { 5, 4, 3 };
    
    PageList* page_list = create_page_list(200);

    printf("\nSHOULD BE EMPTY:\n");
    
    for (int i = 0; i < NUM; i++)
        print_page_entry(search_page_list(not_to_find[i], page_list));
    
    printf("############\n");

    for (int i = 0; i < NUM; i++)
    {
        add_page_list_ord(create_page_entry(addr[i], 1, 1, 1, 1, NULL),
                            page_list, cmp_index);
        print_page_entry(search_page_list(to_find[i], page_list));
        printf("\n");

        print_page_entry(search_page_list(not_to_find[i], page_list));
        printf("\n");
    }

    for (int i = 0; i < NUM; i++)
    {
        print_page_entry(search_page_list(to_find[i], page_list));
        printf("\n");
    }

    printf("\nSHOULD BE EMPTY:\n");
    
    for (int i = 0; i < NUM; i++)
        print_page_entry(search_page_list(not_to_find[i], page_list));
    
    printf("############\n");

    free_page_list(page_list, TRUE);
}

void test_remove_page_list_first(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 };
    
    PageList* page_list = create_page_list(200);
    PageEntry* page_entry;
    

    for (int i = 0; i < NUM; i++)
    {
        add_page_list_ord(create_page_entry(addr[i], 1, 1, 1, 1, NULL),
                            page_list, cmp_index);
    }

    for (int i = 0; i < NUM + 1; i++)
    {
        page_entry = remove_page_list_first(page_list);
        
        if (page_entry != NULL)
            print_page_entry(page_entry);
        else
            printf("!!! EMPTY");

        printf("\n");
    }

    print_page_list(page_list);
    printf("\n");

    free_page_list(page_list, TRUE);
}

void test_remove_page_list_last(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 };
    
    PageList* page_list = create_page_list(200);
    PageEntry* page_entry;
    

    for (int i = 0; i < NUM; i++)
    {
        add_page_list_ord(create_page_entry(addr[i], 1, 1, 1, 1, NULL),
                            page_list, cmp_index);
    }

    for (int i = 0; i < NUM + 1; i++)
    {
        page_entry = remove_page_list_last(page_list);
        
        if (page_entry != NULL)
            print_page_entry(page_entry);
        else
            printf("!!! EMPTY");

        printf("\n");
    }

    print_page_list(page_list);
    printf("\n");

    free_page_list(page_list, TRUE);
}

void test_remove_page_list_index(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 },
                 test[NUM + 2] = { 5, 9, 10, 1, 3 };
    
    PageList* page_list = create_page_list(200);
    PageEntry* page_entry;
    

    for (int i = 0; i < NUM; i++)
    {
        add_page_list_ord(create_page_entry(addr[i], 1, 1, 1, 1, NULL),
                            page_list, cmp_index);
    }

    for (int i = 0; i < NUM + 2; i++)
    {
        page_entry = remove_page_list_index(test[i], page_list);
        
        if (page_entry != NULL)
            print_page_entry(page_entry);
        else
            printf("!!! NOT FOUND");

        printf("\n");
    }

    print_page_list(page_list);
    printf("\n");

    free_page_list(page_list, TRUE);
}

void test_ord_page_list_already_ord(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 };
    
    PageList* page_list = NULL;

    // NULL list
    ord_page_list(page_list, cmp_index);

    if (page_list == NULL)
        printf("* NULL list OK\n");

    // 0 element
    printf("\n* 0 ELEMENT\n");
    page_list = create_page_list(200);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    if (page_list == NULL)
        printf("* Empty list OK\n");

    // 1 element
    printf("\n* 1 ELEMENT\n");
    add_page_list_ord(create_page_entry(addr[0], 1, 1, 1, 1, NULL), page_list, cmp_index);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    // 2 elements
    printf("\n* 2 ELEMENTS\n");
    add_page_list_ord(create_page_entry(addr[1], 1, 1, 1, 1, NULL), page_list, cmp_index);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    // 3 elements
    printf("\n* 3 ELEMENTS\n");
    add_page_list_ord(create_page_entry(addr[2], 1, 1, 1, 1, NULL), page_list, cmp_index);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    free_page_list(page_list, TRUE);
}

void test_ord_page_list_not_ord(void)
{
    unsigned int addr[NUM] = { 10, 9, 1 };
    
    PageList* page_list = NULL;

    // NULL list
    ord_page_list(page_list, cmp_index);

    if (page_list == NULL)
        printf("* NULL list OK\n");

    // 0 element
    printf("\n* 0 ELEMENT\n");
    page_list = create_page_list(200);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    if (page_list == NULL)
        printf("* Empty list OK\n");

    // 1 element
    printf("\n* 1 ELEMENT\n");
    add_page_list_first(create_page_entry(addr[0], 1, 1, 1, 1, NULL), page_list);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    // 2 elements
    printf("\n* 2 ELEMENTS\n");
    add_page_list_first(create_page_entry(addr[1], 1, 1, 1, 1, NULL), page_list);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    // 3 elements
    printf("\n* 3 ELEMENTS\n");
    add_page_list_first(create_page_entry(addr[2], 1, 1, 1, 1, NULL), page_list);
    ord_page_list(page_list, cmp_index);
    print_page_list(page_list);

    free_page_list(page_list, TRUE);
}

int main(void)
{
    // printf("\n*** CREATE PAGE:\n");
    // test_create_page();

    // printf("\n*** CREATE NODE:\n");
    // test_create_node();

    printf("\n*** PAGE LIST:\n");
    // printf("\n### EMPTY:\n");
    // test_create_page_list_empty();

    // printf("\n### ADD FIRST:\n");
    // test_add_page_entry_first();

    // printf("\n### ADD LAST:\n");
    // test_add_page_entry_last();

    // printf("\n### FREE PG TRUE:\n");
    // test_free_page_list(TRUE);

    // printf("\n### FREE PG FALSE:\n");
    // test_free_page_list(FALSE);

    // printf("\n### ADD ORD:\n");
    // test_add_page_list_ord();

    // printf("\n### SEARCH:\n");
    // test_search_page_entry();

    // printf("\n### REMOVE FIRST:\n");
    // test_remove_page_list_first();

    // printf("\n### REMOVE LAST:\n");
    // test_remove_page_list_last();

    // printf("\n### REMOVE INDEX:\n");
    // test_remove_page_list_index();

    printf("\n### ORD PAGE LIST:\n");
    test_ord_page_list_already_ord();

    test_ord_page_list_not_ord();

    return 0;
}
