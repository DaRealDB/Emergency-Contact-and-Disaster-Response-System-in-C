#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CONTACTS 100

typedef struct 
{
    char name[50];
    char type[30];
    char phone[15];
    int priority;
} Contact;

Contact heap[MAX_CONTACTS];
int heapSize = 0;

#define TABLE_SIZE 10
Contact *hashTable[TABLE_SIZE] = {NULL};

void swap(Contact *a, Contact *b) 
{
    Contact temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(int i) 
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if(left < heapSize && heap[left].priority > heap[largest].priority)
        largest = left;
        
    if(right < heapSize && heap[right].priority > heap[largest].priority)
        largest = right;
        
    if(largest != i) 
	{
        swap(&heap[i], &heap[largest]);
        heapify(largest);
    }
}

void insertContact(char name[], char type[], char phone[], int priority) 
{
    if(heapSize >= MAX_CONTACTS) 
	{
        printf("Contact list is full!\n");
        return;
    }

    strcpy(heap[heapSize].name, name);
    strcpy(heap[heapSize].type, type);
    strcpy(heap[heapSize].phone, phone);
    heap[heapSize].priority = priority;

    int i = heapSize;
    heapSize++;

    while(i > 0 && heap[(i - 1) / 2].priority < heap[i].priority) 
	{
        swap(&heap[i], &heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }

    printf("Contact added successfully!\n");
}

void getHighestPriorityContact() 
{
    if(heapSize == 0) 
	{
        printf("No contacts available.\n");
        return;
    }

    printf("\nHighest Priority Contact:\n");
    printf("Name: %s, Type: %s, Phone: %s, Priority: %d\n",
           heap[0].name, heap[0].type, heap[0].phone, heap[0].priority);

    heap[0] = heap[heapSize - 1];
    heapSize--;
    heapify(0);
}

int hashFunction(char *type) 
{
    int sum = 0;
    for(int i = 0; type[i] != '\0'; i++)
        sum += type[i];
    return sum % TABLE_SIZE;
}

void categorizeContact(Contact contact) 
{
    int index = hashFunction(contact.type);
    hashTable[index] = (Contact *)malloc(sizeof(Contact));
    if(hashTable[index] != NULL) 
	{
        *hashTable[index] = contact;
    }
}

void searchByType(char *type) 
{
    int index = hashFunction(type);
    if(hashTable[index] != NULL && strcmp(hashTable[index]->type, type) == 0) 
	{
		
        printf("\nEmergency Contact Found:\n");
        printf("Name: %s, Type: %s, Phone: %s, Priority: %d\n", hashTable[index]->name, hashTable[index]->type, hashTable[index]->phone, hashTable[index]->priority);
        
    } 
	else 
	{
        printf("No contact found for type: %s\n", type);
    }
}

void saveContactsToFile() 
{
    FILE *file = fopen("contacts.txt", "w");
    if(!file) 
	{
        printf("Error saving contacts!\n");
        return;
    }
    for(int i = 0; i < heapSize; i++) 
	{
        fprintf(file, "Contact name:%s, Contact type:%s, Contact number:%s, Priority:%d\n", heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
    fclose(file);
    printf("Contacts saved to file successfully!\n");
}

void loadContactsFromFile() 
{
    FILE *file = fopen("contacts.txt", "r");
    if(!file) 
	{
        printf("");
        return;
    }
    char name[50], type[30], phone[15];
    int priority;
    while (fscanf(file, "%49[^,],%29[^,],%14[^,],%d\n", name, type, phone, &priority) == 4) 
	{
        insertContact(name, type, phone, priority);
    }
    fclose(file);
    printf("Contacts loaded from file successfully!\n");
}

void displayContacts() 
{
    if(heapSize == 0) 
	{
        printf("No contacts available.\n");
        return;
    }
    printf("\nAll Emergency Contacts:\n");
    for(int i = 0; i < heapSize; i++) 
	{
        printf("Name: %s, Type: %s, Phone: %s, Priority: %d\n", heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
}

void menu() 
{
    int choice;
    char name[50], type[30], phone[15];
    int priority;

    do 
	{
        printf("\n.:  Emergency Contact & Disaster Response System :.");
        printf("\n\n1. Add Contact");
		printf("\n2. Get Highest Priority Contact");
		printf("\n3. Search Contact by Type");
        printf("\n4. Display All Contacts");
		printf("\n5. Save Contacts to File");
		printf("\n6. Load Contacts from File");
		printf("\n7. Program Exit\n");
        printf("\nEnter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) 
		{
            case 1:
            	printf("\033[2J\033[H");
                printf("Enter Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                printf("Enter Type (Police, Fire, Hospital): ");
                fgets(type, sizeof(type), stdin);
                type[strcspn(type, "\n")] = '\0';
                printf("Enter Phone: ");
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = '\0';
                printf("Enter Priority (1-10): ");
                scanf("%d", &priority);

                insertContact(name, type, phone, priority);

                Contact newContact;
                strcpy(newContact.name, name);
                strcpy(newContact.type, type);
                strcpy(newContact.phone, phone);
                newContact.priority = priority;
                categorizeContact(newContact);
				printf("\033[2J\033[H");
                break;

            case 2:
            	printf("\033[2J\033[H");
                getHighestPriorityContact();
                break;
                printf("\033[2J\033[H");
				
            case 3:
            	printf("\033[2J\033[H");
                printf("Enter Contact Type to Search: ");
                fgets(type, sizeof(type), stdin);
                type[strcspn(type, "\n")] = '\0';
                searchByType(type);
                break;
				printf("\033[2J\033[H");
            case 4:
            	printf("\033[2J\033[H");
                displayContacts();
                break;
				printf("\033[2J\033[H");
            case 5:
            	printf("\033[2J\033[H");
                saveContactsToFile();
                break;
                printf("\033[2J\033[H");

            case 6:
            	printf("\033[2J\033[H");
                loadContactsFromFile();
                break;
                printf("\033[2J\033[H");
        }
    } while(choice != 7);
}

int main() 
{
    loadContactsFromFile();
    menu();
    return 0;
}
