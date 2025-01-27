/*
TURMA: 3WA

INTEGRANTES:
    * Guilherme Riechert Senko 2011478
    * Pedro de Almeida Barizon 2211350

PROFESSOR: Luiz Fernando Seibel

DATA: 24-11-2024
*/

#include "virtual_mem.h"

/**
 * Gets data stored in argv inputted by user.
 */
void get_data(int argc, char** argv);

/**
 * Updates global variables accordingly to the info
 * obtained in get_data.
 */
void configure_sim(void);

/**
 * Returns the next line in which the page with index is referenced in
 * the address file.
 * 
 * @param index Index to be searched.
 * @param current_ref Current line in which the file is.
 * @param addr_file Pointer to the address file open in read or any equivalent mode.
 * 
 * @return If found, the line of next referenced; else PINF (Page Index Not Found).
 */
unsigned int get_next_ref(unsigned int index, unsigned int current_ref, FILE* addr_file);

/**
 * Increments global page fault counter and adds the newly referenced
 * page to the page list.
 * 
 * @return If a substitution is required, the substituted
 * page is returned, else NULL.
 */
Page* page_fault(unsigned int index, char mode, PageList* page_list);

/**
 * Increments global page write counter and frees
 * the page pointed by *page. Sets *page to NULL.
 */
void page_write(Page** page);

/**
 * Upadates the simulation's list of pages. This routine is
 * activated whenever a new reference occurs and a page fault is
 * not caused.
 */
void list_update(unsigned int index, char mode, PageList* page_list);

/**
 * Runs the paging simulation.
 */
void paging_sim(void);

/**
 * Logs the result of the memory simulation.
 * Prints the values of relevant global variables.
 */
void log_result(void);
