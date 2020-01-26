#include "list.h"

//tworzy nowy węzeł
List_node * list_init(char * name) {
	List_node *tmp = (List_node*)malloc(sizeof(List_node));
	tmp->next = NULL;
    strcpy(tmp->str, name);
    return tmp;
}

//dodaje węzeł do listy
List_node * list_add(List_node * node, char* name)
{
	List_node *tmp= list_init(name);
    if(tmp==NULL)      // brak pamieci
        return NULL;

    tmp->next = node;

    return tmp;
}

// nalezy w argumencie podac wskaznik na head
void list_free(List_node * head)
{
    if(head == NULL)
        return;

    list_free(head->next);
    free(head);
}

//zwraca ile jest węzłow listy
int list_len(List_node * node)
{
	int counter=0;
	while(node!=NULL) {
		counter++;
		node=node->next;
	}
	return counter;
}

//wyświetla liste
void list_print(List_node * tmp)
{
	while(tmp!=NULL) {
		printf("%s\n", tmp->str);
		tmp=tmp->next;
	}	
}

char *list_convert_to_str(List_node *head){
    if (head == NULL) {
        return "";
    }

    size_t size = 0;

    for(List_node * tmp = head; tmp != NULL; tmp=tmp->next){
        size += strlen(tmp->str) + 1;
    }

    char *result = malloc(sizeof(char) * (size + 1));
    result[0] = '\0';

    for(List_node * tmp = head; tmp != NULL; tmp=tmp->next){
        strcat(result, tmp->str);
        strcat(result, " ");
    }

    return result;
}


