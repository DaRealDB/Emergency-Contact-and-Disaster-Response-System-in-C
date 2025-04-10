#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For sleep function
#define MAX_LINE 100
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
void animateText(const char* text, int delay_ms) 
{
    for(i = 0; text[i] != '\0'; i++) 
	{
        printf("%c", text[i]);
        fflush(stdout);
        usleep(delay_ms * 1000);  // Convert to microseconds
    }
    printf("\n");
}

// Function to display a styled header
void displayHeader(const char* title) 
{
    int len = strlen(title);
    printf("\n%s", BLUE);
    for(i = 0; i < len + 10; i++) printf("=");
    printf("\n%s    %s%s    ", BLUE, YELLOW, title);
    printf("\n%s", BLUE);
    for(i = 0; i < len + 10; i++) printf("=");
    printf("%s\n\n", RESET);
}

// Progress bar animation
void showProgressBar(const char* message, int duration_ms) 
{
    printf("%s%s%s\n", CYAN, message, RESET);
    printf("[");
    for(i = 0; i < 20; i++) 
	{
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

    //showProgressBar("Adding contact", 500);
    //printf("%sContact added successfully!%s\n", GREEN, RESET);
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
        fprintf(file, "Contact Name:%s, Contact Type:%s, Contact Number:%s, Priority:%d\n", heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
    fclose(file);
}

void editContact(int index) 
{
    char newName[50], newType[30], newPhone[15];
    int newPriority;

    printf("\n%sEditing Contact:%s %s\n", CYAN, RESET, heap[index].name);
    printf("%sPress Enter to skip a field without changing it.%s\n\n", YELLOW, RESET);

    printf("%sNew Name (Current: %s): %s", YELLOW, heap[index].name, RESET);
    fgets(newName, sizeof(newName), stdin);
    if(newName[0] != '\n') 
    {
        newName[strcspn(newName, "\n")] = '\0'; // Remove newline
        strcpy(heap[index].name, newName);
    }

    printf("%sNew Type (Current: %s): %s", YELLOW, heap[index].type, RESET);
    fgets(newType, sizeof(newType), stdin);
    if(newType[0] != '\n') 
    {
        newType[strcspn(newType, "\n")] = '\0';
        strcpy(heap[index].type, newType);
    }

    printf("%sNew Phone (Current: %s): %s", YELLOW, heap[index].phone, RESET);
    fgets(newPhone, sizeof(newPhone), stdin);
    if(newPhone[0] != '\n') 
    {
        newPhone[strcspn(newPhone, "\n")] = '\0';
        strcpy(heap[index].phone, newPhone);
    }

    printf("%sNew Priority (Current: %d): %s", YELLOW, heap[index].priority, RESET);
    char priorityInput[10];
    fgets(priorityInput, sizeof(priorityInput), stdin);
    if(priorityInput[0] != '\n') 
    {
        sscanf(priorityInput, "%d", &newPriority);
        heap[index].priority = newPriority;
    }

	saveContactsToFile();
    printf("\n%sContact updated successfully!%s\n", GREEN, RESET);
}

void searchByType(char *type) 
{
    int found = 0;
    int contactIndexes[MAX_CONTACTS];
    int count = 0;
    int choice;

    displayHeader("Search Results");
    animateText("Searching for contacts...", 20);

    for(i = 0; i < heapSize; i++) 
    {
        char tempType[30];
        strcpy(tempType, heap[i].type);
        tempType[strcspn(tempType, "\n")] = '\0'; // Remove trailing newline if any

        if(strcasecmp(tempType, type) == 0)  // Case-insensitive comparison
        {
            contactIndexes[count++] = i;
            if(!found) 
            {
                printf("%sContacts of type '%s':%s\n\n", GREEN, type, RESET);
                found = 1;
            }

            printf("[%d] +---------------------------------+\n", count);
            printf("    | %sName:%s %-23s |\n", CYAN, RESET, heap[i].name);
            printf("    | %sType:%s %-23s |\n", CYAN, RESET, heap[i].type);
            printf("    | %sPhone:%s %-22s |\n", CYAN, RESET, heap[i].phone);
            printf("    | %sPriority:%s %-19d |\n", CYAN, RESET, heap[i].priority);
            printf("    +---------------------------------+\n\n");
        }
    }

    if(!found) 
    {
        printf("%sNo contacts found for type: %s%s\n", RED, type, RESET);
        return;
    }

    // Ask if the user wants to edit a contact
    printf("%sDo you want to edit a contact? (1 - Yes, 0 - No): %s", YELLOW, RESET);
    scanf("%d", &choice);
    getchar();  // Consume newline

    if(choice == 1) 
    {
        int editIndex;
        printf("%sEnter the number of the contact to edit:%s ", YELLOW, RESET);
        scanf("%d", &editIndex);
        getchar();  

        if(editIndex >= 1 && editIndex <= count) 
        {
            editContact(contactIndexes[editIndex - 1]);
        } 
        else 
        {
            printf("%sInvalid selection!%s\n", RED, RESET);
        }
    }
}

void loadContactsFromFile() 
{
    FILE *file = fopen("contacts.txt", "r");
    if(!file) 
    {
        printf("%sNo saved contacts found.%s\n", RED, RESET);
        return;
    }

    char line[200]; // Buffer to read each line
    char name[50], type[30], phone[15];
    int priority;
    
    // Clear existing contacts
    heapSize = 0;

    while(fgets(line, sizeof(line), file)) 
    {
        // Extract data while skipping the labels
        if(sscanf(line, "Contact Name:%49[^,], Contact Type:%29[^,], Contact Number:%14[^,], Priority:%d",
                   name, type, phone, &priority) == 4) 
        {
            insertContact(name, type, phone, priority);
        }
    }
    fclose(file);
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
    printf("� %-20s %-15s %-15s %-8s �\n", "Name", "Type", "Phone", "Priority");
    printf("+-----------------------------------------------------------�\n");
    
    for(i = 0; i < heapSize; i++) 
    {
        printf("� %-20s %-15s %-15s %-8d �\n", 
               heap[i].name, heap[i].type, heap[i].phone, heap[i].priority);
    }
    
    printf("+-----------------------------------------------------------+\n");
}

void displaySplashScreen() 
{
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
    for(i = 0; i < 30; i++) 
	{
        printf("=");
        fflush(stdout);
        usleep(50000);  // 50ms delay
    }
    printf("] 100%%\n\n");
    
    printf("%sPress Enter to continue...%s", YELLOW, RESET);
    getchar();
    printf("\033[2J\033[H");
}

void DisasterReportLogging() 
{
    char report[500];
    FILE *file = fopen("disaster_reports.txt", "a");  // Open file in append mode

    if(!file) 
	{
        printf("%sError opening disaster report file!%s\n", RED, RESET);
        return;
    }

    displayHeader("Log a Disaster Report");
    
    printf("%sEnter your disaster report (max 500 characters):%s\n", YELLOW, RESET);
    fgets(report, sizeof(report), stdin);
    report[strcspn(report, "\n")] = '\0'; // Remove newline character
    
    fprintf(file, "%s\n", report);  // Save report to file
    fclose(file);
    
    showProgressBar("Saving disaster report", 800);
    printf("%sDisaster report logged successfully!%s\n", GREEN, RESET);
}

void DisplayDisasterReports() 
{
    char report[500];
    FILE *file = fopen("disaster_reports.txt", "r");

    if (!file) {
        printf("%sNo disaster reports found.%s\n", RED, RESET);
        return;
    }

    displayHeader("All Disaster Reports");
    while (fgets(report, sizeof(report), file)) {
        printf("%s- %s%s\n", CYAN, report, RESET);
    }
    fclose(file);
}


void DeleteContact() 
{
    if(heapSize == 0) 
	{
        printf("%sNo contacts available to delete.%s\n", RED, RESET);
        return;
    }

    displayContacts(); // Show available contacts

    int deleteIndex;
    printf("%sEnter the index of the contact to delete (1-%d): %s", YELLOW, heapSize, RESET);
    scanf("%d", &deleteIndex);
    getchar();  // Consume newline

    if (deleteIndex < 1 || deleteIndex > heapSize) {
        printf("%sInvalid index!%s\n", RED, RESET);
        return;
    }

    deleteIndex--; // Convert to zero-based index

    printf("%sDeleting contact: %s%s\n", RED, heap[deleteIndex].name, RESET);

    // Replace deleted element with last element
    heap[deleteIndex] = heap[heapSize - 1];
    heapSize--;

    // Restore heap property
    heapify(deleteIndex);

    showProgressBar("Deleting contact", 500);
    printf("%sContact deleted successfully!%s\n", RED, RESET);
}


void menu() 
{
    int choice;
    char name[50], type[30], phone[15];
    int priority;


    do 
    {
        displayHeader("Emergency Contact & Disaster Response System");
        
        printf("%s 1 %s Add New Contact\n", YELLOW, RESET);
        printf("%s 2 %s Search Contact by Type\n", YELLOW, RESET);
        printf("%s 3 %s Display All Contacts\n", YELLOW, RESET);
        //printf("%s 4 %s Load Contacts from File\n", YELLOW, RESET);
        printf("%s 4 %s Delete a Contact\n", YELLOW, RESET);
        printf("%s 5 %s Exit Program\n\n", RED, RESET);
        
        printf("%sEnter your choice [1-5]:%s ", CYAN, RESET);
        scanf("%d", &choice);
        getchar();  // Consume newline

        

        switch(choice) 
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
                saveContactsToFile();
                
                printf("\n%sContact Added Successfully! Press Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;

            case 2:
            	printf("\033[2J\033[H");
                displayHeader("Search Contact by Type");
                printf("%sEnter Contact Type to Search:%s ", YELLOW, RESET);
                fgets(type, sizeof(type), stdin);
                type[strcspn(type, "\n")] = '\0';
                searchByType(type);
                saveContactsToFile();        
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;
                
            case 3:
            	printf("\033[2J\033[H");
                displayContacts();
                saveContactsToFile();
                printf("\n%sPress Enter to continue...%s", CYAN, RESET);
                getchar();
                printf("\033[2J\033[H");
                break;
                printf("\033[2J\033[H");
			
			case 4:
				printf("\033[2J\033[H");
    			displayHeader("Delete a Contact");
    			DeleteContact();
    			saveContactsToFile();
    			printf("\n%sPress Enter to continue...%s", CYAN, RESET);
    			getchar();
    			printf("\033[2J\033[H");
   	 			break;
   	 			
                
            case 5:
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
    } while(choice != 6);
}




void speedDial() {
    printf("\033[2J\033[H"); // Clear screen
    
    // Display emergency types
    printf("+--------------------------------------------------+\n");
    printf("|              Select Emergency Type               |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  1. Fire           4. Earthquake     7. Blackout |\n");
    printf("|  2. Flood          5. Tsunami        8. Terror   |\n");
    printf("|  3. Accident       6. Landslide      9. More     |\n");
    printf("+--------------------------------------------------+\n");
    
    int emergencyType;
    printf("Enter the type of emergency you're having (1-9): ");
    scanf("%d", &emergencyType);
    getchar(); // Consume newline
    
    // Map the emergency type to contact type
    char *typeMap[] = {
        "", // 0 - unused
        "Fire", "Flood", "Accident", "Earthquake", 
        "Tsunami", "Landslide", "Blackout", "Terror"
    };
    
    char selectedType[30];
    if (emergencyType >= 1 && emergencyType <= 8) {
        strcpy(selectedType, typeMap[emergencyType]);
    } else if (emergencyType == 9) {
        // More options
        printf("\n+--------------------------------------------------+\n");
        printf("|              Additional Emergency Types         |\n");
        printf("+--------------------------------------------------+\n");
        printf("| 10. Pandemic       13. Avalanche      16. Hazard |\n");
        printf("| 11. Wildfire       14. Blizzards      17. Volcanic Eruption |\n");
        printf("| 12. Drought        15. Tornado        18. Chemical Spill    |\n");
        printf("+--------------------------------------------------+\n");
        printf("Enter additional choice (10-18): ");
        scanf("%d", &emergencyType);
        getchar();
        
        char *moreTypes[] = {
            "", "", "", "", "", "", "", "", "", "", // 0-9 unused
            "Pandemic", "Wildfire", "Drought", "Avalanche", 
            "Blizzards", "Tornado", "Hazard", "Volcanic Eruption", 
            "Chemical Spill"
        };
        
        if (emergencyType >= 10 && emergencyType <= 18) {
            strcpy(selectedType, moreTypes[emergencyType]);
        } else {
            printf("Invalid choice!\n");
            return;
        }
    } else {
        printf("Invalid choice!\n");
        return;
    }
    
    // Find all contacts of the selected type
    int matchingContacts[MAX_CONTACTS];
    int matchCount = 0;
    
    for (int i = 0; i < heapSize; i++) {
        if (strcasecmp(heap[i].type, selectedType) == 0) {
            matchingContacts[matchCount++] = i;
        }
    }
    
    if (matchCount == 0) {
        printf("\nNo contacts found for %s emergencies!\n", selectedType);
        printf("Press Enter to return to main menu...");
        getchar();
        return;
    }
    
    // Display matching contacts sorted by priority
    printf("\n+--------------------------------------------------+\n");
    printf("| Available %s Emergency Contacts (by priority) |\n", selectedType);
    printf("+--------------------------------------------------+\n");
    
    // Sort by priority (since it's a max-heap, the first element is highest priority)
    for (int i = 0; i < matchCount; i++) {
        int contactIdx = matchingContacts[i];
        printf("| %d. %-20s Priority: %-8d |\n", 
               i+1, heap[contactIdx].name, heap[contactIdx].priority);
        printf("|    Phone: %-36s |\n", heap[contactIdx].phone);
        printf("+--------------------------------------------------+\n");
    }
    
    // Let user select contact
    int contactChoice;
    printf("\nSelect the contact to notify (1-%d): ", matchCount);
    scanf("%d", &contactChoice);
    getchar();
    
    if (contactChoice < 1 || contactChoice > matchCount) {
        printf("Invalid selection!\n");
        return;
    }
    
    int selectedContactIdx = matchingContacts[contactChoice-1];
    
    // Display confirmation
    printf("\n\033[1;32m"); // Green text
    printf("Your location has been sent to %s (%s).\n", 
           heap[selectedContactIdx].name, heap[selectedContactIdx].phone);
    printf("\033[0m"); // Reset color
    
    // Save this emergency action to file
    FILE *logFile = fopen("emergency_logs.txt", "a");
    if (logFile) {
        fprintf(logFile, "Emergency Type: %s\n", selectedType);
        fprintf(logFile, "Contacted: %s (%s)\n", 
                heap[selectedContactIdx].name, heap[selectedContactIdx].phone);
        fprintf(logFile, "Time: %s\n", __TIME__);
        fprintf(logFile, "----------------------------------------\n");
        fclose(logFile);
    }
    
    printf("\nPress Enter to return to main menu...");
    getchar();
}


void mainMenu(){
	displaySplashScreen();
	  printf("+-------------------------+-------------------------+\n");
    printf("| Emergency              | Emergency               |\n");
    printf("| speed dial             | Contacts                |\n");
    printf("|                        |                         |\n");
    printf("| type [1]               | type [2]                |\n");
    printf("+-------------------------+-------------------------+\n");
	printf("Enter choice: ");
    int choice;
    scanf("%d", &choice);
    switch(choice){
    	case 1: 
    		speedDial();
    		break;
    	case 2:
    		printf("\033[2J\033[H");
    	 	menu();
    		break;
    	default:
    		printf("Enter valid Choice");
    		break;
	}
}





int main() 
{
    // Initialize hash table
    for(i = 0; i < TABLE_SIZE; i++) 
	{
        hashTable[i] = NULL;
    }
    
    loadContactsFromFile();
    mainMenu();
    
    return 0;
}
