#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define MAX_SEATS 15

struct airline
{
    char passport[10];
    char name[30];
    char destination[30];
    char flight_name[20];
    char reservation_time[20]; 
    int seat_num;
    char seat_prefix;
    char email[40];
    struct airline *following;
};

struct airline *begin = NULL;


struct flight_tracker
{
    char destination[30];
    char flight_name[20];
    int count;
    char prefix;
    struct flight_tracker *next;
};

struct flight_tracker *flight_begin = NULL;


void clearScreen() { system("cls"); }
void pauseScreen() { printf("\n\tPress any key to continue..."); getch(); }

void printHeader(const char *title)
{
    printf("\n============================================================\n");
    printf("\t\t%s\n", title);
    printf("============================================================\n");
}


struct flight_tracker* get_flight_tracker(char *destination)
{
    struct flight_tracker *temp = flight_begin;
    while (temp)
    {
        if (strcmp(temp->destination, destination) == 0)
            return temp;
        temp = temp->next;
    }

    const char *airlines[] = { "US-Bangla", "Novoair", "Air Astra", "SkyAir" };
    int airline_count = sizeof(airlines)/sizeof(airlines[0]);
    srand(time(NULL) + rand());
    int random_airline = rand() % airline_count;

    struct flight_tracker *new_tracker = (struct flight_tracker*)malloc(sizeof(struct flight_tracker));
    strcpy(new_tracker->destination, destination);
    strcpy(new_tracker->flight_name, airlines[random_airline]);
    new_tracker->count = 0;
    new_tracker->prefix = 'A' + (flight_begin ? flight_begin->count : 0);
    new_tracker->next = flight_begin;
    flight_begin = new_tracker;

    return new_tracker;
}


void get_details(struct airline *p)
{
    printf("\nEnter Passport Number (max 9 chars): ");
    fgets(p->passport, sizeof(p->passport), stdin);
    p->passport[strcspn(p->passport, "\n")] = 0;

    printf("Enter Full Name: ");
    fgets(p->name, sizeof(p->name), stdin);
    p->name[strcspn(p->name, "\n")] = 0;

    printf("Enter Email Address: ");
    fgets(p->email, sizeof(p->email), stdin);
    p->email[strcspn(p->email, "\n")] = 0;

    printf("Enter Destination: ");
    fgets(p->destination, sizeof(p->destination), stdin);
    p->destination[strcspn(p->destination, "\n")] = 0;

    struct flight_tracker *tracker = get_flight_tracker(p->destination);
    tracker->count++;
    p->seat_num = tracker->count;
    p->seat_prefix = tracker->prefix;
    strcpy(p->flight_name, tracker->flight_name);

    // Only ask for date
    printf("Enter Reservation Date (DD/MM/YY): ");
    fgets(p->reservation_time, sizeof(p->reservation_time), stdin);
    p->reservation_time[strcspn(p->reservation_time, "\n")] = 0;
}


void reserve()
{
    struct airline *new_node = (struct airline *)malloc(sizeof(struct airline));
    if (!new_node)
    {
        printf("\nError: Memory allocation failed!\n");
        return;
    }

    printHeader("NEW RESERVATION");
    get_details(new_node);
    new_node->following = NULL;

    if (!begin) begin = new_node;
    else
    {
        struct airline *temp = begin;
        while (temp->following) temp = temp->following;
        temp->following = new_node;
    }

    printf("\nReservation successful!");
    printf("\nSeat Number      : %c-%d", new_node->seat_prefix, new_node->seat_num);
    printf("\nFlight Name      : %s", new_node->flight_name);
    printf("\nDestination      : %s", new_node->destination);
    printf("\nReservation Date : %s\n", new_node->reservation_time);
}


void display()
{
    if (!begin)
    {
        printf("\nNo bookings found.\n");
        return;
    }

    printHeader("CURRENT BOOKINGS");
    printf("%-6s %-20s %-20s %-15s %-20s %-10s %-12s\n",
           "Seat", "Name", "Flight", "Destination", "Email", "Passport", "Res. Date");
    printf("---------------------------------------------------------------------------------------\n");

    struct airline *temp = begin;
    while (temp)
    {
        printf("%c-%-5d %-20s %-20s %-15s %-20s %-10s %-12s\n",
               temp->seat_prefix, temp->seat_num, temp->name, temp->flight_name,
               temp->destination, temp->email, temp->passport, temp->reservation_time);
        temp = temp->following;
    }
}


void view_individual()
{
    if (!begin)
    {
        printf("\nNo bookings found.\n");
        return;
    }

    char passport_input[10];
    printHeader("VIEW INDIVIDUAL RECORD");
    printf("Enter Passport Number: ");
    fgets(passport_input, sizeof(passport_input), stdin);
    passport_input[strcspn(passport_input, "\n")] = 0;

    struct airline *temp = begin;
    while (temp)
    {
        if (strcmp(temp->passport, passport_input) == 0)
        {
            printf("\nSeat Number      : %c-%d", temp->seat_prefix, temp->seat_num);
            printf("\nName             : %s", temp->name);
            printf("\nEmail            : %s", temp->email);
            printf("\nDestination      : %s", temp->destination);
            printf("\nFlight Name      : %s", temp->flight_name);
            printf("\nReservation Date : %s", temp->reservation_time);
            printf("\nPassport         : %s\n", temp->passport);
            return;
        }
        temp = temp->following;
    }

    printf("\nNo record found for passport number: %s\n", passport_input);
}


void cancel()
{
    if (!begin)
    {
        printf("\nNo bookings to cancel.\n");
        return;
    }

    char passport_input[10];
    printHeader("CANCEL RESERVATION");
    printf("Enter Passport Number to cancel: ");
    fgets(passport_input, sizeof(passport_input), stdin);
    passport_input[strcspn(passport_input, "\n")] = 0;

    struct airline *temp = begin, *prev = NULL;

    while (temp)
    {
        if (strcmp(temp->passport, passport_input) == 0)
        {
            if (prev == NULL) begin = temp->following;
            else prev->following = temp->following;
            free(temp);
            printf("\nBooking cancelled successfully.\n");
            return;
        }
        prev = temp;
        temp = temp->following;
    }

    printf("\nPassport number not found!\n");
}

void savefile()
{
    FILE *fp = fopen("air_records.txt", "w");
    if (!fp)
    {
        printf("\nError opening file to save records.\n");
        return;
    }

    struct airline *temp = begin;
    while (temp)
    {
        fprintf(fp, "%s,%s,%s,%s,%c-%d,%s\n",
                temp->passport,
                temp->name,
                temp->flight_name,
                temp->destination,
                temp->seat_prefix,
                temp->seat_num,
                temp->reservation_time);
        temp = temp->following;
    }

    fclose(fp);
}


void loadfile()
{
    FILE *fp = fopen("air_records.txt", "r");
    if (!fp) return; // If file does not exist, just return

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        struct airline *new_node = (struct airline*)malloc(sizeof(struct airline));
        if (!new_node) continue;

        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%c-%d,%[^\n]",
               new_node->passport,
               new_node->name,
               new_node->flight_name,
               new_node->destination,
               &new_node->seat_prefix,
               &new_node->seat_num,
               new_node->reservation_time);

        new_node->following = NULL;

        if (!begin) begin = new_node;
        else
        {
            struct airline *temp = begin;
            while (temp->following) temp = temp->following;
            temp->following = new_node;
        }

        // Update flight_tracker count
        struct flight_tracker *tracker = get_flight_tracker(new_node->destination);
        if (tracker->count < new_node->seat_num)
            tracker->count = new_node->seat_num;
    }

    fclose(fp);
}


int main()
{
    loadfile(); // <-- Load existing reservations

    int choice;
    do
    {
        clearScreen();
        printf("\n============================================================");
        printf("\n\t           AIRLINE BOOKING SYSTEM           ");
        printf("\n============================================================\n");
        printf("1. Reservation\n");
        printf("2. Cancel Booking\n");
        printf("3. View All Records\n");
        printf("4. View Individual Record\n");
        printf("5. Exit\n");
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            choice = 0;
        }
        fflush(stdin);
        clearScreen();

        switch (choice)
        {
        case 1:
            reserve();
            break;
        case 2:
            cancel();
            break;
        case 3:
            display();
            break;
        case 4:
            view_individual();
            break;
        case 5:
            savefile();
            printf("\nThank you for using the Airline Booking System!\n");
            break;
        default:
            printf("Invalid choice! Please enter a number between 1 and 5.\n");
            break;
        }
        pauseScreen();
    }
    while (choice != 5);

    return 0;
}

