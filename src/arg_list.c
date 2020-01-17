#include "arg_list.h"

//tworzy nowy węzeł
Arg_node * init_arg(char * name) {
	Arg_node *tmp = (Arg_node*)malloc(sizeof(Arg_node));
	tmp->next = NULL;
    strcpy(tmp->arg, name);
    return tmp;
}

//dodaje węzeł do listy
Arg_node * arg_list_add(Arg_node * head, char* name)
{
	Arg_node *tmp=init_arg(name);
    if(tmp==NULL)      // brak pamieci
        return NULL;

    tmp->next = head;

    return tmp;
}

// nalezy w argumencie podac wskaznik na head
void arg_list_remove_all(Arg_node * tmp)
{
    if(tmp == NULL)
        return;

    arg_list_remove_all(tmp->next);
	tmp->next=NULL;
    free(tmp);
}

//zwraca ile jest węzłow listy
int arg_list_count(Arg_node * node)
{
	int counter=0;
	while(node!=NULL) {
		counter++;
		node=node->next;
	}
	return counter;
}

//wyświetla liste
void print_arg_list(Arg_node * tmp)
{
	while(tmp!=NULL) {
		printf(" * %s\n", tmp->arg);
		tmp=tmp->next;
	}	
}

