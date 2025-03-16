#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For sleep function

#define MAX_CONTACTS 100
#define TABLE_SIZE 10

// ANSI color codes
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"

typedef struct 
{
    char name[50];
    char type[30];
    char phone[15];
    int priority;
} Contact;

Contact heap[MAX_CONTACTS];
int heapSize = 0;

Contact *hashTable[TABLE_SIZE] = {NULL};
int i;

// Function for animated text printing
void animateText(const char* text, int delay_ms) {
    for(i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        usleep(delay_ms * 1000);  // Convert to microseconds
    }
    printf("\n");
}

// Function to display a styled header
void displayHeader(const char* title) {
    int len = strlen(title);
    printf("\n%s", BLUE);
    for(i = 0; i < len + 10; i++) printf("=");
    printf("\n%s    %s%s    ", BLUE, YELLOW, title);
    printf("\n%s", BLUE);
    for(i = 0; i < len + 10; i++) printf("=");
    printf("%s\n\n", RESET);
}

// Progress bar animation
void showProgressBar(const char* message, int duration_ms) {
    printf("%s%s%s\n", CYAN, message, RESET);
    printf("[");
    for (i = 0; i < 20; i++) {
        printf("#");
        fflush(stdout);
        usleep(duration_ms * 1000 / 20);
    }
    printf("] %sDone!%s\n", GREEN, RESET);
}

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
        printf("%sContact list is full!%s\n", RED, RESET);
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

    showProgressBar("Adding contact", 500);
    printf("%sContact added successfully!%s\n", GREEN, RESET);
}

void getHighestPriorityContact() 
{
    if(heapSize == 0) 
    {
        printf("%sNo contacts available.%s\n", RED, RESET);
        return;
    }

    displayHeader("Highest Priority Contact");
    printf("%sName:%s %s\n", YELLOW, RESET, heap[0].name);
    printf("%sType:%s %s\n", YELLOW, RESET, heap[0].type);
    printf("%sPhone:%s %s\n", YELLOW, RESET, heap[0].phone);
    printf("%sPriority:%s %d\n", YELLOW, RESET, heap[0].priority);

    heap[0] = heap[heapSize - 1];
    heapSize--;
    heapify(0);
}

int hashFunction(char *type) 
{
    int sum = 0;
    for(i = 0; type[i] != '\0'; i++)
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
    int found = 0;  // Flag to track if any matching contacts were found
    
    // Search through the entire heap array for contacts of the specified type
    displayHeader("Search Results");
    animateText("Searching for contacts...", 20);
    
    for(i = 0; i < heapSize; i++) 
    {
        if(strcmp(heap[i].type, type) == 0) 
        {
            // Display found contact with formatting
            if (!found) {
                printf("%sContacts of type '%s':%s\n\n", GREEN, type, RESET);
                found = 1;
            }
            
            printf("+---------------------------------+\n");
            printf("¦ %sName:%s %-23s ¦\n", CYAN, RESET, heap[i].name);
            printf("¦ %sType:%s %-23s ¦\n", CYAN, RESET, heap[i].type);
            printf("¦ %sPhone:%s %-22s ¦\n", CYAN, RESET, heap[i].phone);
            printf("¦ %sPriority:%s %-19d ¦\n", CYAN, RESET, heap[i].priority);
            printf("+---------------------------------+\n\n");
        }
    }
    
    if(!found) 
    {
        printf("%sNo contacts found for type: %s%s\n", RED, type, RESET);
    }
}

void saveContactsToFile() 
{
    FILE *file = fopen("contacts.txt", "w");
    if(!file) 
    {
        printf("%sError saving contacts!%s\n", RED, RESET);
        return;
    }
    for(i = 0; i < heapSize; i++) 
    {
        fprintf(file, "%s,%s,%s,%d\n", heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
    fclose(file);
    showProgressBar("Saving contacts to file", 800);
    printf("%sContacts saved to file successfully!%s\n", GREEN, RESET);
}

void loadContactsFromFile() 
{
    FILE *file = fopen("contacts.txt", "r");
    if(!file) 
    {
        // File doesn't exist yet - not an error for first run
        return;
    }
    
    char name[50], type[30], phone[15];
    int priority;
    
    // Show loading animation
    animateText("Loading contacts from file...", 20);
    
    // Clear existing contacts
    heapSize = 0;
    
    while (fscanf(file, "%49[^,],%29[^,],%14[^,],%d\n", name, type, phone, &priority) == 4) 
    {
        insertContact(name, type, phone, priority);
    }
    fclose(file);
    printf("%sContacts loaded successfully!%s\n", GREEN, RESET);
}

void displayContacts() 
{
    if(heapSize == 0) 
    {
        printf("%sNo contacts available.%s\n", RED, RESET);
        return;
    }
    
    displayHeader("All Emergency Contacts");
    
    printf("+-----------------------------------------------------------+\n");
    printf("¦ %-20s %-15s %-15s %-8s ¦\n", "Name", "Type", "Phone", "Priority");
    printf("+-----------------------------------------------------------¦\n");
    
    for(i = 0; i < heapSize; i++) 
    {
        printf("¦ %-20s %-15s %-15s %-8d ¦\n", 
               heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
    
    printf("+-----------------------------------------------------------+\n");
}

void displaySplashScreen() {
	printf("\033[2J\033[H");
	
    printf("%s", BLUE);
    printf("  _____                                              \n");
    printf(" | ____|  _ __ ___     ___   _ __    __ _    ___   _ __     ___   _   _ \n");
    printf(" |  _|   | '_ ` _ \\   / _ \\ | '__|  / _` |  / _ \\ | '_ \\   / __| | | | |\n");
    printf(" | |___  | | | | | | |  __/ | |    | (_| | |  __/ | | | | | (__  | |_| |\n");
    printf(" |_____| |_| |_| |_|  \\___| |_|     \\__, |  \\___| |_| |_|  \\___|  \\__, |\n");
    printf("                                    |___/                         |___/ \n");
    printf("\n");
    printf("   _____                  _                  _    \n");
    printf("  / ____|                | |                | |   \n");
    printf(" | |       ___   _ __   | |_    __ _    ___| |_  \n");
    printf(" | |      / _ \\ | '_ \\  | __|  / _` |  / __| __| \n");
    printf(" | |____ | (_) || | | | | |_  | (_| | | (__| |_  \n");
    printf("  \\_____| \\___/ |_| |_|  \\__|  \\__,_|  \\___|\\__| \n");
    printf("\n");
    printf("   _____                 _                        \n");
    printf("  / ____|               | |                       \n");
    printf(" | (___   _   _   ___  | |_    ___   _ __ ___    \n");
    printf("  \\___ \\ | | | | / __| | __|  / _ \\ | '_ ` _ \\  \n");
    printf("  ____) || |_| | \\__ \\ | |_  |  __/ | | | | | | \n");
    printf(" |_____/  \\__, | |___/  \\__|  \\___| |_| |_| |_| \n");
    printf("           __/ |                                 \n");
    printf("          |___/                                  \n");
    printf("%s", RESET);
    
    animateText("   !!  DISASTER RESPONSE SYSTEM  !!", 40);
    printf("\n");
    animateText("           Loading system...", 30);
    
    // Progress bar
    printf("[");
    for (i = 0; i < 30; i++) {
        printf("=");
        fflush(stdout);
        usleep(50000);  // 50ms delay
    }
    printf("] 100%%\n\n");
    
    printf("%sPress Enter to continue...%s", YELLOW, RESET);
    getchar();
    printf("\033[2J\033[H");
}

void menu() 
{
    int choice;
    char name[50], type[30], phone[15];
    int priority;

    displaySplashScreen();

    do 
    {
        displayHeader("Emergency Contact & Disaster Response System");
        
        printf("%s 1 %s Add New Contact\n", YELLOW, RESET);
        printf("%s 2 %s Get Highest Priority Contact\n", YELLOW, RESET);
        printf("%s 3 %s Search Contact by Type\n", YELLOW, RESET);
        printf("%s 4 %s Display All Contacts\n", YELLOW, RESET);
        printf("%s 5 %s Save Contacts to File\n", YELLOW, RESET);
        printf("%s 6 %s Load Contacts from File\n", YELLOW, RESET);
        printf("%s 7 %s Exit Program\n\n", RED, RESET);
        
        printf("%sEnter your choice [1-7]:%s ", CYAN, RESET);
        scanf("%d", &choice);
        getchar();  // Consume newline

        

        switch (choice) 
        {
            case 1:
            	printf("\033[2J\033[H");
                displayHeader("Add New Emergency Contact");
                printf("%sEnter Name:%s ", YELLOW, RESET);
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                
                printf("%sEnter Type (Police, Fire, Hospital, etc.):%s ", YELLOW, RESET);
                fgets(type, sizeof(type), stdin);
                type[strcspn(type, "\n")] = '\0';
                
                printf("%sEnter Phone:%s ", YELLOW, RESET);
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = '\0';
                
                printf("%sEnter Priority (1-10):%s ", YELLOW, RESET);
                scanf("%d", &priority);
                getchar();  // Consume newline

                insertContact(name, type, phone, priority);

                Contact newContact;
                strcpy(newContact.name, name);
                strcpy(newContact.type, type);
                strcpy(newContact.phone, phone);
                newContact.priority = priority;
                categorizeContact(newContact);
                
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;

            case 2:
            	printf("\033[2J\033[H");
                getHighestPriorityContact();
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                break;
                printf("\033[2J\033[H");
            case 3:
            	printf("\033[2J\033[H");
                displayHeader("Search Contact by Type");
                printf("%sEnter Contact Type to Search:%s ", YELLOW, RESET);
                fgets(type, sizeof(type), stdin);
                type[strcspn(type, "\n")] = '\0';
                searchByType(type);
                
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;
                
            case 4:
            	printf("\033[2J\033[H");
                displayContacts();
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;
                printf("\033[2J\033[H");
            case 5:
            	printf("\033[2J\033[H");
                saveContactsToFile();
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;

            case 6:
            	printf("\033[2J\033[H");
                loadContactsFromFile();
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;
                
            case 7:
                displayHeader("Exiting Program");
                animateText("Thank you for using Emergency Contact System!", 30);
                animateText("Saving your data...", 30);
                saveContactsToFile();
                animateText("Goodbye!", 50);
                break;
                
            default:
                printf("%sInvalid choice! Please try again.%s\n", RED, RESET);
                usleep(1500000);  // 1.5 second delay
                printf("\033[2J\033[H");
        }
    } while(choice != 7);
}

int main() 
{
    // Initialize hash table
    for(i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
    
    loadContactsFromFile();
    menu();
    
    return 0;
}
