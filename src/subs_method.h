#include "virtual_mem.h"

/* LEAST RECENTLY USED */
void    lru_add(PageList* page_list, PageEntry* page_entry);
Page*   lru_subs(PageList* page_list);
void    lru_update(int index, char mode, unsigned int time, PageList* page_list);

/* FIRST IN FIRST OUT WITH SECOND CHANCE (FIFO SC) */
void    sc_add(PageList* page_list, PageEntry* page_entry);
Page*   sc_subs(PageList* page_list);
void    sc_update(int index, char mode, PageList* page_list);

/* NOT RECENTLY USED */
void    nru_add(PageList* page_list, PageEntry* page_entry);
Page*   nru_subs(PageList* page_list);
void    nru_update(int index, char mode, unsigned int time, PageList* page_list);

/* OPTIMAL ALGORITHM */
void    optimal_add(PageList* page_list, PageEntry* page_entry);
Page*   optimal_subs(PageList* page_list);
void    optimal_update(int index, char mode, unsigned int time, unsigned int next_ref, PageList* page_list);
