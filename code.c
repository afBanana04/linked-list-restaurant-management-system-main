#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_RED     "\033[0;31m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RESET   "\033[0;37m"

void clearScreen() {
    system("clear");
}

void clearBuffer(){
    int c;
    while((c = getchar()) != '\n' && c!= EOF);
}

struct node{
    int data;
    char foodname[100];
    float price;
    struct node *next;
};

struct order_item{
    char foodname[100];
    int quantity;
    float price;
    struct order_item *next;
};

struct table{
    int table_number;
    int capacity;
    int is_available; // 0: unavailable, 1: available
    struct table *next;
};

struct order{
    int id;
    char customer_name[20];
    struct order_item *items;
    float total;
    time_t timestamp;
    int status; // 0: pending, 1: completed, 2: serving
    int table_number;
    struct order *next;
};

int password;
int i = 0;
int menu_id;
int order_id;

struct node *head_a = NULL;
struct order *head_o = NULL;
struct order *head_h = NULL;
struct order *head_s = NULL;
struct table *head_table = NULL;

void clearBuffer();
void initialize_tables();
void display_available_tables();
void save_tables_to_file();
void load_tables_from_file();
void load_menu(struct node **head);
void mainmenu();
void displaymenu(struct node *head_a);
void sort_menu_by_price(struct node **head_a);
void display_menu_sorted_by_price(struct node *head_a);
void write_order_to_file(struct order *order, const char *filename);
void show_order_details(struct order *new_order);
struct order *create_order(struct table *temp_table);
void add_order_item(struct order *order, struct order_item *new_item);
struct order_item *create_order_item(struct node *menu_item, int quantity); 
void place_order();
void customermenu();
void customer();
void adminmenu();
void addmenu(struct node **head_a);
void save_menu(struct node **head_a);
void deletemenu(struct node **head_a);
struct node *linear_search(struct node *head_a, int item_number);
void edit_menu_item_price(struct node *head_a);
void adminmenu1();
void adminmenu2();
void manageorders();
void managemenuitems();
void changepassword();
void authenticate();
void admin();
void load_orders(const char *file_name, struct order **head);
void adminmenu3();
void managehistory();
void display_orders(struct order *head);
void update_orders(const char *filename, struct order *head);
void delete_and_save_order(struct order **head,int up_status, const char *update_filename, const char *delete_filename);
void update_table_status(int table_number, int status);
struct node *find_menu_item(struct node *head, int item_id);
void display_order_detail(struct order *order);
void update_order(struct order *target_order, struct node *menu_head);
void delete_order(struct order **head, struct order *target_order);
struct order* search_order(struct order *head, const char *customer_name, int table_number);
void save_current_id_to_file(const char *filename, int current_id);
int load_current_id_from_file(const char *filename);
void save_item_to_menu(struct node *newNode);
struct node *merge(struct node *left, struct node *right);
void split(struct node *source, struct node **frontRef, struct node **backRef);

int load_current_id_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading!\n");
        exit(1); 
    }
    int current_id;
    if (fscanf(file, "%d", &current_id) != 1) {
        printf("Error reading current ID from file!\n");
        fclose(file);
        exit(1); 
    }
    fclose(file);
    return current_id;
}

void save_current_id_to_file(const char *filename, int current_id) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        exit(1);
    }
    fprintf(file, "%d", current_id);
    fclose(file);
}

void initialize_default_tables() {
    struct table *new_table;
    for (int i = 1; i <= 26; i++) {
        new_table = malloc(sizeof(struct table));
        if (new_table == NULL) {
            printf(COLOR_RED "\t\t\tMemory allocation failed for new table.\n" COLOR_RESET);
            return;
        }
        new_table->table_number = i;
        if (i <= 15) {
            new_table->capacity = 2;
        } else if (i <= 25) {
            new_table->capacity = 4;
        } else {
            new_table->capacity = 6;
        }
        new_table->is_available = 1;
        new_table->next = head_table;
        head_table = new_table;
    }
}

void save_tables_to_file(){
    FILE *file = fopen("tables.txt","w");
    if(file == NULL){
        printf(COLOR_RED "Unable to open file for saving table data.\n" COLOR_RESET);
        return;
    }

    struct table *temp_table = head_table;
    while (temp_table != NULL)
    {
        fprintf(file,"%d %d %d\n", temp_table->table_number, temp_table->capacity,temp_table->is_available);
        temp_table = temp_table->next;
    }
    fclose(file);
}

void load_tables_from_file(){
    FILE *file = fopen("tables.txt", "r");
    if(file == NULL){
        printf(COLOR_YELLOW "No saved table data found. Initializing default tables.\n" COLOR_RESET);
        initialize_default_tables();
        save_tables_to_file();
        return;
    }

    head_table = NULL;

    int table_number, capacity, is_available;
    while(fscanf(file, "%d %d %d", &table_number, &capacity, &is_available) == 3){
        struct table *new_table = malloc(sizeof(struct table));
        if(new_table == NULL){
            printf(COLOR_RED "Error: Memory allocation failed for a new table!\n" COLOR_RESET);
            fclose(file);
            return;
        }
        new_table->table_number = table_number;
        new_table->capacity = capacity;
        new_table->is_available = is_available;
        new_table->next = head_table;
        head_table = new_table;
    }

    fclose(file);

    printf("Tables loaded successfully.\n");
}

void display_available_tables(){
    clearScreen();
    printf(COLOR_YELLOW"\t\t\t%-10s %-10s %-10s\n", "Table No.", "Capacity", "Status");
    printf("\t\t\t------------------------------------\n"COLOR_RESET);

    struct table *temp_table = head_table;
    while(temp_table != NULL){
        if(temp_table->is_available){
            printf("\t\t\t%-10d %-10d %-10s\n", temp_table->table_number, temp_table->capacity, "Available");
        }else{
            printf("\t\t\t%-10d %-10d " COLOR_RED "%-10s\n" COLOR_RESET, temp_table->table_number, temp_table->capacity, "Occupied");
        }
        temp_table = temp_table->next;
    }
}

void load_menu(struct node **head) {
    FILE *file = fopen("menu.txt", "r");
    if (file == NULL) {
        printf(COLOR_RED "Error opening menu file!\n" COLOR_RESET);
        return;
    }

    struct node *current = NULL;
    int data;
    char foodname[50];
    float price;

    while (fscanf(file, "%d,%49[^,],%f", &data, foodname, &price) == 3) {
        struct node *new_node = (struct node *)malloc(sizeof(struct node));
        if (new_node == NULL) {
            printf(COLOR_RED "Memory allocation failed!\n" COLOR_RESET);
            fclose(file);
            return;
        }

        new_node->data = data;
        strcpy(new_node->foodname, foodname);
        new_node->price = price;
        new_node->next = NULL;

        if (*head == NULL) {
            *head = new_node;
        } else {
            current = *head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_node;
        }
    }

    fclose(file);
}


void mainmenu(){
    printf(COLOR_YELLOW);
    printf("\n\n\t\t\t Press "COLOR_RED "1"COLOR_YELLOW " for ADMIN\n");
    printf("\t\t\t Press "COLOR_RED "2"COLOR_YELLOW" to CUSTOMER\n\n");
    printf("\t\t\t Press "COLOR_RED "0"COLOR_YELLOW " to EXIT\n\n");
    printf(COLOR_RESET);
    printf("\t\t\t Enter your choice ---->  ");
}

void displaymenu(struct node *head_a) {
    if (head_a == NULL) {
        printf(COLOR_RED "Menu is empty!\n" COLOR_RESET);
        return;
    }

    printf(COLOR_YELLOW);
    printf("\n\n\t\t\t------------------------------------------------------------------------------\n");
    printf("\t\t\tSr\t\tFood Item\t\t\tPrice\n");
    printf("\t\t\t------------------------------------------------------------------------------\n");

    struct node *current = head_a;
    int count = 1;
    while (current != NULL) {
        printf("\t\t\t%d.\t\t%-30s\t%.2f\n", count, current->foodname, current->price);
        current = current->next;
        count++;
    }
    printf("\t\t\t-------------------------------------------------------------------------------\n\n\n" COLOR_RESET);
}


struct node *merge(struct node *left, struct node *right) {
    struct node *result = NULL;

    if (left == NULL) {
        return right;
    } else if (right == NULL) {
        return left;
    }
    if (left->price <= right->price) {
        result = left;
        result->next = merge(left->next, right);
    } else {
        result = right;
        result->next = merge(left, right->next);
    }

    return result;
}

void sort_menu_by_price(struct node **headRef) {
    struct node *head = *headRef;
    struct node *left, *right;

    if (head == NULL || head->next == NULL) {
        return;
    }

    split(head, &left, &right);

    sort_menu_by_price(&left);
    sort_menu_by_price(&right);

    *headRef = merge(left, right);
}

void split(struct node *source, struct node **frontRef, struct node **backRef) {
    struct node *fast, *slow;
    slow = source;
    fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

void display_menu_sorted_by_price(struct node *head_a) {
    clearScreen();
    if (head_a == NULL) {
        printf(COLOR_RED "Menu is empty!\n" COLOR_RESET);
        return;
    }
    struct node *copy_head = NULL;
    struct node *current = head_a;
    while (current != NULL) {
        struct node *newNode = (struct node *)malloc(sizeof(struct node));
        if (newNode == NULL) {
            printf(COLOR_RED "Memory allocation failed!\n" COLOR_RESET);
            exit(1);
        }
        newNode->data = current->data;
        strcpy(newNode->foodname, current->foodname);
        newNode->price = current->price;
        newNode->next = copy_head;
        copy_head = newNode;
        current = current->next;
    }

    sort_menu_by_price(&copy_head);

    printf(COLOR_YELLOW); 
    printf("\n\n\t\t\t------------------------------------------------------------------------------\n");
    printf("\t\t\tSr\t\tFood Item\t\t\tPrice\n");
    printf("\t\t\t------------------------------------------------------------------------------\n");
    current = copy_head;
    while (current != NULL) {
        printf("\t\t\t%d.\t\t%-30s\t%.2f\n", current->data, current->foodname, current->price);
        current = current->next;
    }
    printf("\t\t\t-------------------------------------------------------------------------------\n\n\n"COLOR_RESET);

    while (copy_head != NULL) {
        struct node *temp = copy_head;
        copy_head = copy_head->next;
        free(temp);
    }
}

void write_order_to_file(struct order *order, const char *filename) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fprintf(file, "%d,%s,%.2f,%ld,%d,%d", order->id, order->customer_name, order->total, order->timestamp, order->status, order->table_number);
        struct order_item* item = order->items;
        while (item != NULL) {
            fprintf(file, ",%s,%d,%.2f", item->foodname, item->quantity, item->price); 
            item = item->next;
        }
        fprintf(file, "\n");
        fclose(file);
    } else {
        printf("Failed to open file for writing.\n");
    }
}


void add_order_item(struct order *order, struct order_item *new_item) {
    new_item->next = order->items;
    order->items = new_item;
    order->total += new_item->price;
}

struct order_item *create_order_item(struct node *menu_item, int quantity) {
    struct order_item *new_item = malloc(sizeof(struct order_item));
    if (new_item == NULL) {
        printf("Memory allocation failed for new item.\n");
        return NULL;
    }

    strcpy(new_item->foodname, menu_item->foodname);
    new_item->quantity = quantity;
    new_item->price = menu_item->price * quantity;
    new_item->next = NULL;

    return new_item;
}

struct order *create_order(struct table *temp_table) {
    struct order *new_order = malloc(sizeof(struct order));
    if (new_order == NULL) {
        printf("Memory allocation failed for new order.\n");
        return NULL;
    }

    printf("Enter customer name:");
    char customer_name[100];
    clearBuffer();
    fgets(customer_name, sizeof(customer_name), stdin);
    if (customer_name[strlen(customer_name) - 1] == '\n') {
        customer_name[strlen(customer_name) - 1] = '\0';
    }
    strcpy(new_order->customer_name, customer_name);

    new_order->id = order_id++;
    new_order->items = NULL;
    new_order->total = 0;
    new_order->status = 0;
    new_order->timestamp = time(NULL);
    new_order->table_number = temp_table->table_number;

    save_current_id_to_file("order_id.txt",order_id);
    return new_order;
}

void show_order_details(struct order *new_order) {
    printf("\n\t\t\t\t== Order Details ==\n");
    printf("\t\t\t---------------------------------------\n");
    printf("\t\t\tOrder ID: %d\n", new_order->id);
    printf("\t\t\tCustomer: %s\n", new_order->customer_name);
    printf("\t\t\tTotal: %.2f\n", new_order->total);
    printf("\t\t\tOrder time: %s", ctime(&(new_order->timestamp)));
    printf("\t\t\tStatus: Pending\n");
    printf("\t\t\tTable: %d\n", new_order->table_number);
    printf("\t\t\t----------------------------------------\n");
    printf("\t\t\tItems:\n");

    struct order_item *item = new_order->items;
    while (item != NULL) {
        printf("\t\t\t%s x%d - %.2f\n", item->foodname, item->quantity, item->price);
        item = item->next;
    }
    printf("\t\t\t-----------------------------------------\n");
}

void place_order() {
    clearScreen();

    int chosen_capacity;
    printf("Enter the table's size you want (2, 4, or 6): ");
    scanf("%d", &chosen_capacity);

    struct table *temp_table = head_table;
    int table_found = 0;
    while (temp_table != NULL) {
        if (temp_table->is_available && temp_table->capacity == chosen_capacity) {
            table_found = 1;
            break;
        }
        temp_table = temp_table->next;
    }

    if (!table_found) {
        printf("Sorry, there are no available tables for you.\n");
        return;
    }

    displaymenu(head_a);
    struct order *new_order = create_order(temp_table);
    if (new_order == NULL) {
        return;
    }

    char choice;
    do {
        int menu_id, quantity;
        printf("Enter item ID: ");
        scanf("%d", &menu_id);

        struct node *menu_item_ptr = linear_search(head_a, menu_id);
        if (menu_item_ptr == NULL) {
            printf("Item not found in menu.\n");
            free(new_order);
            return;
        }

        printf("Enter quantity: ");
        scanf("%d", &quantity);

        struct order_item *new_item = create_order_item(menu_item_ptr, quantity);
        if (new_item == NULL) {
            free(new_order);
            return;
        }

        add_order_item(new_order, new_item);

        printf("Add another item? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    new_order->next = head_o;
    head_o = new_order;
    clearScreen();
    printf("Order placed successfully!\n");

    show_order_details(new_order);

    write_order_to_file(new_order, "pending_orders.txt");

    temp_table->is_available = 0;
    save_tables_to_file();
    load_orders("pending_orders.txt",&head_o);
}


void customermenu(){
    printf(COLOR_YELLOW"\t\t\tPress "COLOR_RED "1"COLOR_YELLOW" to DISPLAY menu\n");
    printf("\t\t\tPress "COLOR_RED "2"COLOR_YELLOW" to DISPLAY menu(sorted by price)\n");
    printf("\t\t\tPress "COLOR_RED "3"COLOR_YELLOW" to DISPLAY tables status\n");
    printf("\t\t\tPress "COLOR_RED "4"COLOR_YELLOW" to PLACE an order\n");
    printf("\t\t\tPress "COLOR_RED "0"COLOR_YELLOW" to EXIT to main menu\n");
    printf(COLOR_BLUE"\t\t\t Enter your choice ---->  "COLOR_RESET);
}

void customer(){
    while (1) {
        int choice;
        customermenu();
        printf(COLOR_YELLOW);
        scanf("%d", &choice);
        printf(COLOR_RESET);
        if (choice == 0) {
            clearScreen();
            break;
        }
        switch (choice) {
            case 1:
                clearScreen();
                displaymenu(head_a);
                break;
            case 2:
                display_menu_sorted_by_price(head_a);
                break;
            case 3:
                display_available_tables();
                break;
            case 4:
                place_order();
                break;
            default:
                printf("Incorrect Choice! \nPlease choose a valid option.\n");
                break;
        }
    }
}

void adminmenu() {
    printf(COLOR_YELLOW"\t\t\tPress "COLOR_RED "1"COLOR_YELLOW" to Manage Menu Items\n");
    printf("\t\t\tPress "COLOR_RED "2"COLOR_YELLOW" to Manage Orders\n");
    printf("\t\t\tPress "COLOR_RED "3"COLOR_YELLOW" to Display Tables Status\n");
    printf("\t\t\tPress "COLOR_RED "4"COLOR_YELLOW" to View Orders History\n");
    printf("\t\t\tPress "COLOR_RED "5"COLOR_YELLOW" to Change Admin Password\n");
    printf("\t\t\tPress "COLOR_RED "0"COLOR_YELLOW" to Exit to Main Menu\n");
    printf(COLOR_BLUE"\t\t\t Enter Your Choice ---->  "COLOR_RESET);
}

void addmenu(struct node **head_a) {
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    if (newNode == NULL) {
        printf(COLOR_RED "Memory allocation failed!\n" COLOR_RESET);
        exit(1);
    }

    printf("\n\t\t\tEnter food name: ");
    printf(COLOR_RED);
    char foodname[100];
    clearBuffer();
    fgets(foodname, sizeof(foodname), stdin);
    if (foodname[strlen(foodname) - 1] == '\n') {
        foodname[strlen(foodname) - 1] = '\0';
    }
    strcpy(newNode->foodname, foodname);
    printf(COLOR_RESET);

    printf("\t\t\tEnter price: ");
    printf(COLOR_RED);
    scanf("%f", &newNode->price);
    printf(COLOR_RESET);

    newNode->data = menu_id++;

    newNode->next = NULL;
    if (*head_a == NULL) {
        *head_a = newNode;
    } else {
        struct node *last = *head_a;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newNode;
    }

    save_item_to_menu(newNode);
    save_current_id_to_file("menu_id.txt", menu_id);

    printf(COLOR_BLUE "\n\t\t\tFood item added successfully!\n" COLOR_RESET);
}

void save_item_to_menu(struct node *newNode) {
    FILE *file = fopen("menu.txt", "a"); 
    if (file == NULL) {
        printf(COLOR_RED "Unable to open file for saving menu data.\n" COLOR_RESET);
        return;
    }

    fprintf(file, "%d,%s,%.2f\n", newNode->data, newNode->foodname, newNode->price);

    fclose(file);
}

void save_menu(struct node **head_a) {
    FILE *file = fopen("menu.txt", "w");
    if (file == NULL) {
        printf(COLOR_RED "Unable to open file for saving menu data.\n" COLOR_RESET);
        return;
    }

    struct node *temp = *head_a;
    while (temp != NULL) {
        fprintf(file, "%d,%s,%.2f\n", temp->data, temp->foodname, temp->price);
        temp = temp->next;
    }

    fclose(file);
}

void deletemenu(struct node **head_a) {
    if (*head_a == NULL) {
        printf(COLOR_RED "Menu is empty!\n" COLOR_RESET);
        return;
    }

    displaymenu(*head_a);

    int data;
    printf("Enter the number of item to be deleted : ");
    printf(COLOR_BLUE);
    if (scanf("%d", &data) <= 0) {
        printf(COLOR_RED "Invalid input! Please enter a valid item number.\n" COLOR_RESET);
        clearBuffer();
        return;
    }

    struct node *toDelete = linear_search(*head_a, data);

    if (toDelete == NULL) {
        printf(COLOR_RED "Item not found!\n" COLOR_RESET);
        return;
    }

    struct node *prev = NULL;
    struct node *current = *head_a;

    while (current != toDelete) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        *head_a = current->next;
    } else {
        prev->next = current->next;
    }

    if (current->next == NULL) {
        menu_id--;
        save_current_id_to_file("menu_id.txt", menu_id);
    }

    free(current);

    printf(COLOR_RED "\nItem Deleted!\n" COLOR_RESET);
    save_menu(head_a); 
}

struct node *linear_search(struct node *head_a, int item_number) {
    struct node *current = head_a;

    while (current != NULL) {
        if (current->data == item_number) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


void edit_menu_item_price(struct node *head_a) {
    if (head_a == NULL) {
        printf(COLOR_RED "Menu is empty!\n" COLOR_RESET);
        return;
    }

    printf("Current Menu:\n");
    displaymenu(head_a);

    int data;
    printf("Enter the item number to edit its price: ");
    printf(COLOR_BLUE);
    if (scanf("%d", &data) <= 0) {
        printf(COLOR_RED "Invalid input! Please enter a valid item number.\n" COLOR_RESET);
        clearBuffer();
        return;
    }
    struct node *current = linear_search(head_a, data);
    if (current != NULL) {
        printf("\nCurrent price of %s: %.2f\n", current->foodname, current->price);
        printf("Enter the new price: ");
        float new_price;
        printf(COLOR_RED);
        if (scanf("%f", &new_price) <= 0 || new_price < 0) {
            printf(COLOR_RED "Invalid input! Please enter a valid price.\n" COLOR_RESET);
            clearBuffer();
            return;
        }
        printf(COLOR_RESET);

        current->price = new_price;
        printf(COLOR_BLUE "Price updated successfully!\n" COLOR_RESET);

        save_menu(&head_a);
        return;
    } else {
        printf(COLOR_RED "Item not found!\n" COLOR_RESET);
    }
}

void adminmenu1(){
    printf(COLOR_YELLOW"\t\t\tPress "COLOR_RED "1"COLOR_YELLOW" to ADD a new item\n");
    printf("\t\t\tPress "COLOR_RED "2"COLOR_YELLOW" to DELETE an item from menu\n");
    printf("\t\t\tPress "COLOR_RED "3"COLOR_YELLOW" to EDIT item price\n");
    printf("\t\t\tPress "COLOR_RED "0"COLOR_YELLOW" to EXIT to admin menu\n");
    printf("\033[0m");
    printf(COLOR_BLUE"\t\t\t Enter your choice ---->  "COLOR_RESET);
}

void adminmenu2(){
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "1" COLOR_YELLOW " to DISPLAY PENDING orders\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "2" COLOR_YELLOW " to DISPLAY SERVING orders\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "3" COLOR_YELLOW " to MARK order SERVED\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "4" COLOR_YELLOW " to MARK order COMPLETED\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "5" COLOR_YELLOW " to UPDATE an order\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "6" COLOR_YELLOW " to DELETE an order\n");
    printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "0" COLOR_YELLOW " to EXIT\n");
    printf(COLOR_BLUE "\t\t\tEnter your choice ----> " COLOR_RESET);
}

void manageorders() {
    while (1) {
        int choice;
        adminmenu2();
        printf(COLOR_BLUE);
        scanf("%d", &choice);
        clearBuffer(); 
        printf(COLOR_RESET);
        if (choice == 0) {
            break;
        }
        char customer_name[100];
        int table_number;
        struct order *target_order;
        switch (choice) {
            case 1:
                clearScreen();
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t\t== PENDING ORDERS ==\n");
                display_orders(head_o);
                break;
            case 2:
                clearScreen();
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t\t== SERVING ORDERS ==\n");
                display_orders(head_s);
                break;
            case 3:
                clearScreen();
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t\t== PENDING ORDERS ==\n");
                display_orders(head_o);
                delete_and_save_order(&head_o, 2, "pending_orders.txt", "order_serving.txt");
                load_orders("order_serving.txt",&head_s);
                load_orders("pending_orders.txt",&head_o);
                break;
            case 4:
                clearScreen();
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t\t== SERVING ORDERS ==\n");
                display_orders(head_s);
                delete_and_save_order(&head_s, 1, "order_serving.txt", "order_history.txt");
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t\t== SERVING ORDERS ==\n");
                clearScreen();
                display_orders(head_s);
                load_orders("order_history.txt",&head_h);
                load_tables_from_file();
                break;
            case 5:
                printf("Enter customer name: ");
                fgets(customer_name, sizeof(customer_name), stdin);
                if (customer_name[strlen(customer_name) - 1] == '\n') {
                    customer_name[strlen(customer_name) - 1] = '\0';
                }

                printf("Enter table number: ");
                scanf("%d", &table_number);
                clearBuffer();
                target_order = search_order(head_o, customer_name, table_number);

                if (target_order == NULL) {
                    printf(COLOR_RED "Order not found!\n" COLOR_RESET);
                } else {
                    update_order(target_order,head_a);
                }
                break;
            case 6:
                printf("Enter customer name: ");
                fgets(customer_name, sizeof(customer_name), stdin);
                if (customer_name[strlen(customer_name) - 1] == '\n') {
                    customer_name[strlen(customer_name) - 1] = '\0';
                }
                printf("Enter table number: ");
                scanf("%d", &table_number);
                clearBuffer();
                target_order = search_order(head_o, customer_name, table_number);
                if (target_order == NULL) {
                    printf(COLOR_RED "Order not found!\n" COLOR_RESET);
                } else {
                    delete_order(&head_o, target_order);
                }
                break;
            default:
                printf(COLOR_RED "Incorrect Choice! Please choose a valid option.\n" COLOR_RESET);
                break;
        }
    }
}

void managemenuitems(){
    displaymenu(head_a); 
    while (1) {
        int choice;
        adminmenu1();
        printf(COLOR_BLUE);
        scanf("%d", &choice);
        printf(COLOR_RESET);
        if (choice == 0) {
            clearScreen();
            break;
        }
        switch (choice) {
            case 1:
                addmenu(&head_a);
                clearScreen();
                displaymenu(head_a); 
                break;
            case 2:
                clearScreen();
                deletemenu(&head_a);
                clearScreen();
                displaymenu(head_a);
                break;
            case 3:
                clearScreen();
                edit_menu_item_price(head_a);
                displaymenu(head_a);
                break;
            default:
                printf(COLOR_RED"Incorrect Choice! \nPlease choose a valid option.\n"COLOR_RESET);
                break;
        }
    }
}

void changepassword() {
    char newPassword[100];
    printf("\n\t\t\tEnter your new password : ");
    printf(COLOR_RED);
    scanf("%s", newPassword);
    password = atoi(newPassword); 
    printf(COLOR_BLUE);
    printf("\n\t\t\tPassword changed successfully!\n");
    printf(COLOR_RESET);
    save_current_id_to_file("password.txt",password);
}

void authenticate(){
    printf("\n\t\t\tEnter the password : ");
    int pass;
    printf(COLOR_RED);
    scanf("%d",&pass);
    printf(COLOR_RESET"\n");
    clearBuffer();
    if(pass == password){
        clearScreen();
        admin();
    }else{
        printf(COLOR_RED"Wrong Password!\n"COLOR_RESET);
    }
}

void admin(){
    while(1){
        int choice;
        adminmenu();
        printf(COLOR_BLUE);
        scanf("%d", &choice);
        printf(COLOR_RESET);
        if (choice == 0){
            clearScreen();
            break;
        }
        switch(choice){
            case 1:
                clearScreen();
                managemenuitems();
                break;
            case 2:
                clearScreen();
                manageorders();
                break;
            case 3:
                clearScreen();
                display_available_tables();
                break;
            case 4:
                clearScreen();
                printf(COLOR_YELLOW "\n\t\t\t\t\t\t== ORDERS HISTORY ==\n");
                display_orders(head_h);
                break;
            case 5:
                clearScreen();
                changepassword();
                break;
            default:
                printf(COLOR_RED"Incorrect Choice!\n Please choose a valid option.\n");
                break;
        }
    }
}

void load_orders(const char *file_name, struct order **head) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        printf(COLOR_RED "Error opening %s!\n" COLOR_RESET, file_name);
        return;
    }

    struct order *current_order = NULL;

    int id, status, table_number;
    char customer_name[100];
    float total;
    time_t timestamp;

    while (*head != NULL) {
        struct order *temp = *head;
        *head = (*head)->next;
        struct order_item *item = temp->items;
        while (item != NULL) {
            struct order_item *temp_item = item;
            item = item->next;
            free(temp_item);
        }
        free(temp);
    }

    while (fscanf(file, "%d,%99[^,],%f,%ld,%d,%d\n", &id, customer_name, &total, &timestamp, &status, &table_number) == 6) {
        struct order *new_order = (struct order*)malloc(sizeof(struct order));
        if (new_order == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }

        new_order->id = id;
        strcpy(new_order->customer_name, customer_name);
        new_order->total = total;
        new_order->timestamp = timestamp;
        new_order->status = status;
        new_order->table_number = table_number;
        new_order->items = NULL;
        new_order->next = NULL;

        if (*head == NULL) {
            *head = new_order;
            current_order = *head;
        } else {
            current_order->next = new_order;
            current_order = current_order->next;
        }

        struct order_item *current_item = NULL;
        char foodname[100];
        int quantity;
        float price;
        while (fscanf(file, ",%99[^,],%d,%f", foodname, &quantity, &price) == 3) {
            struct order_item *new_item = (struct order_item*)malloc(sizeof(struct order_item));
            if (new_item == NULL) {
                printf("Memory allocation failed!\n");
                return;
            }

            strcpy(new_item->foodname, foodname);
            new_item->quantity = quantity;
            new_item->price = price;
            new_item->next = NULL;

            if (current_order->items == NULL) {
                current_order->items = new_item;
                current_item = current_order->items;
            } else {
                current_item->next = new_item;
                current_item = current_item->next;
            }
        }
    }

    fclose(file);
}

void display_orders(struct order *head) {
    printf("\t============================================================================================================\n");
    printf("\tID\tCustomer Name\tTotal\t\tTime\t\t\tStatus\t\tTable\tItems\n");
    printf("\t============================================================================================================\n");
    printf(COLOR_RESET);

    struct order *current = head;
    while (current != NULL) {
        struct tm *tm_info = localtime(&current->timestamp);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("\t%d\t%-15s\t%-8.2f\t%-19s\t%-10s\t%d\n", 
               current->id, 
               current->customer_name, 
               current->total, 
               time_str,
               current->status == 0 ? "Pending" : (current->status == 1 ? "Completed" : "Serving"), 
               current->table_number);

        struct order_item *item = current->items;
        while (item != NULL) {
            printf("\t\t\t\t\t\t\t\t\t\t\t\t%s x%d - %.2f\n", 
                   item->foodname, 
                   item->quantity, 
                   item->price);
            item = item->next;
        }
        printf("\n");
        printf("\t------------------------------------------------------------------------------------------------------------\n");

        current = current->next;
    }
}

void delete_and_save_order(struct order **head, int up_status, const char *update_filename, const char *delete_filename) {
    int check = -1;
    if (up_status == 1) {
        check = 1;
    }
    int order_id;
    printf("Enter the order ID to mark: ");
    printf(COLOR_BLUE);
    scanf("%d", &order_id);
    getchar();
    printf(COLOR_RESET);

    struct order *current = *head;
    struct order *previous = NULL;
    while (current != NULL && current->id != order_id) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf(COLOR_RED "Order with ID %d not found!\n" COLOR_RESET, order_id);
        return;
    }
    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }
    current->status = up_status;
    
    write_order_to_file(current, delete_filename);

    if (check == 1) {
        struct table *temp_table = head_table;
        while (temp_table != NULL) {
            if (temp_table->table_number == current->table_number) {
                temp_table->is_available = 1;
                break;
            }
            temp_table = temp_table->next;
        }
        update_table_status(current->table_number, 1);
    }
    printf(COLOR_BLUE"Marked Successfully\n");
    printf(COLOR_RESET);
    struct order_item *current_item = current->items;
    while (current_item != NULL) {
        struct order_item *temp_item = current_item;
        current_item = current_item->next;
        free(temp_item);
    }
    free(current);
    update_orders(update_filename, *head);
    
}

void update_orders(const char *filename, struct order *head) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf(COLOR_RED "Error opening orders file for writing!\n" COLOR_RESET);
        return;
    }

    struct order *current = head;
    while (current != NULL) {
        fprintf(file, "%d,%s,%.2f,%ld,%d,%d", current->id, current->customer_name, current->total,
                current->timestamp, current->status, current->table_number);

        struct order_item *item = current->items;
        while (item != NULL) {
            fprintf(file, ",%s,%d,%.2f", item->foodname, item->quantity, item->price);
            item = item->next;
        }
        fprintf(file, "\n");

        current = current->next;
    }

    fclose(file);
}

void update_table_status(int table_number, int status) {
    FILE *file = fopen("tables.txt", "r+");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int current_table_number;
    int current_capacity;
    int current_status;

    while (fscanf(file, "%d %d %d", &current_table_number, &current_capacity, &current_status) == 3) {
        if (current_table_number == table_number) {
            fseek(file, -2, SEEK_CUR);
            fprintf(file, " %d", status);
            break; 
        }
    }

    fclose(file);
}

struct order* search_order(struct order *head, const char *customer_name, int table_number) {
    struct order *current = head;
    while (current != NULL) {
        if (strcmp(current->customer_name, customer_name) == 0 && current->table_number == table_number) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void display_order_detail(struct order *order) {
    if (order == NULL) {
        printf("Order is NULL.\n");
        return;
    }

    printf("\t============================================================================================================\n");
    printf("\tID\tCustomer Name\tTotal\t\tTime\t\t\tStatus\t\tTable\tItems\n");
    printf("\t============================================================================================================\n");
    printf(COLOR_RESET);

    struct tm *tm_info = localtime(&order->timestamp);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\t%d\t%-15s\t%-8.2f\t%-19s\t%-10s\t%d\n", 
           order->id, 
           order->customer_name, 
           order->total,
           time_str,
           order->status == 0 ? "Pending" : (order->status == 1 ? "Completed" : "Serving"), 
           order->table_number);

    struct order_item *item = order->items;
    while (item != NULL) {
        printf("\t\t\t\t\t\t\t\t\t\t\t\t%s x%d - %.2f\n", 
               item->foodname, 
               item->quantity, 
               item->price);
        item = item->next;
    }
    printf("\n");
    printf("\t------------------------------------------------------------------------------------------------------------\n");
}

void update_order(struct order *target_order, struct node *menu_head) {
    float initial_total = target_order->total;
    while (1) {
        printf("Order Information:\n");
        display_order_detail(target_order);
        int choice;
        printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "1" COLOR_YELLOW " to ADD a new item to the order\n");
        printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "2" COLOR_YELLOW " to UPDATE an item's quantity\n");
        printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "3" COLOR_YELLOW " to DELETE an item from the order\n");
        printf(COLOR_YELLOW "\t\t\tPress " COLOR_RED "0" COLOR_YELLOW " to EXIT update order menu\n");
        printf(COLOR_BLUE "\t\t\tEnter your choice ----> " COLOR_RESET);
        scanf("%d", &choice);
        clearBuffer();

        if (choice == 0) {
            clearScreen();
            break;
        }
        switch (choice) {
            case 1:
            {   
                displaymenu(head_a);
                int item_id;
                printf("Enter item ID: ");
                scanf("%d", &item_id);
                clearBuffer();
                struct node *menu_item = linear_search(menu_head, item_id);
                if (menu_item == NULL) {
                    printf(COLOR_RED "Item not found in the menu!\n" COLOR_RESET);
                    break;
                }
                struct order_item *new_item = (struct order_item *)malloc(sizeof(struct order_item));
                strcpy(new_item->foodname, menu_item->foodname);
                printf("Enter quantity: ");
                scanf("%d", &(new_item->quantity));
                clearBuffer();
                new_item->price = (menu_item->price)*(new_item->quantity);
                new_item->next = NULL;

                if (target_order->items == NULL) {
                    target_order->items = new_item;
                } else {
                    struct order_item *current = target_order->items;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_item;
                }
                target_order->total += new_item->price;

                printf(COLOR_BLUE "Item added successfully!\n" COLOR_RESET);
                break;
            }
            case 2:
            {
                char foodname[100];
                printf("\nEnter food name: ");
                fgets(foodname, sizeof(foodname), stdin);
                if (foodname[strlen(foodname) - 1] == '\n') {
                    foodname[strlen(foodname) - 1] = '\0';
                }
                struct order_item *item = target_order->items;
                while (item != NULL && strcmp(item->foodname, foodname) != 0) {
                    item = item->next;
                }
                if (item == NULL) {
                    printf(COLOR_RED "Item not found!\n" COLOR_RESET);
                } else {
                    int old_quantity = item->quantity;
                    printf("Enter new quantity: ");
                    int new_quantity;
                    scanf("%d", &new_quantity);
                    clearBuffer();
                    
                    float unit_price = item->price / old_quantity;
                    float new_item_price = unit_price * new_quantity;

                    item->quantity = new_quantity;
                    item->price = new_item_price;

                    target_order->total -= (old_quantity * unit_price); 
                    target_order->total += new_item_price; 
                    printf(COLOR_BLUE "Quantity updated successfully!\n" COLOR_RESET);
                }
                break;
            }
            case 3:
            {
                char foodname[100];
                printf("\nEnter food name: ");
                fgets(foodname, sizeof(foodname), stdin);
                if (foodname[strlen(foodname) - 1] == '\n') {
                    foodname[strlen(foodname) - 1] = '\0';
                }
                struct order_item *item = target_order->items;
                struct order_item *prev_item = NULL;
                while (item != NULL && strcmp(item->foodname, foodname) != 0) {
                    prev_item = item;
                    item = item->next;
                }
                if (item == NULL) {
                    printf(COLOR_RED "Item not found!\n" COLOR_RESET);
                } else {
                    if (prev_item == NULL && item->next == NULL) {
                        char confirm;
                        printf("This is the last item in the order. Are you sure you want to delete this order? (y/n): ");
                        scanf(" %c", &confirm);
                        clearBuffer();
                        if (confirm == 'y' || confirm == 'Y') {
                            delete_order(&head_o, target_order);
                            return;
                        } else {
                            break;
                        }
                    } else {
                        target_order->total -= item->price;

                        if (prev_item == NULL) {
                            target_order->items = item->next;
                        } else {
                            prev_item->next = item->next;
                        }

                        free(item);

                        printf(COLOR_BLUE "Item deleted successfully!\n" COLOR_RESET);
                    }
                }
                break;
            }
            default:
                printf(COLOR_RED "Incorrect Choice! Please choose a valid option.\n" COLOR_RESET);
                break;
        }
    }
        float new_total = target_order->total;

        float remaining_due = initial_total - new_total;
        float surplus = new_total - initial_total;

        clearScreen();

        printf("\n\t\t\tInitial Total: $%.2f\n", initial_total);
        printf("\t\t\tNew Total: $%.2f\n", new_total);
        if (remaining_due > 0) {
            printf(COLOR_RED"\t\t\tAmount Due: $%.2f\n"COLOR_RESET, remaining_due);
        } else if (surplus > 0) {
            printf(COLOR_BLUE"\t\t\tAmount Surplus: $%.2f\n"COLOR_RESET, surplus);
        } else {
            printf("\t\t\tNo change in total amount.\n");
        }
    update_orders("pending_orders.txt", head_o);
}


void delete_order(struct order **head, struct order *target_order) {
    struct order *current = *head;
    struct order *prev = NULL;

    while (current != NULL && current != target_order) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf(COLOR_RED "Order not found!\n" COLOR_RESET);
        return;
    }

    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    struct order_item *item = current->items;
    while (item != NULL) {
        struct order_item *temp = item;
        item = item->next;
        free(temp);
    }

    int table_number = current->table_number;

    free(current);
    printf(COLOR_BLUE "Order deleted successfully!\n" COLOR_RESET);

    update_table_status(table_number, 1); 
    update_orders("pending_orders.txt", *head);
    load_tables_from_file();
}

int main() {
    load_tables_from_file();
    load_menu(&head_a);
    menu_id = load_current_id_from_file("menu_id.txt");
    order_id = load_current_id_from_file("order_id.txt");
    password =  load_current_id_from_file("password.txt");
    load_orders("pending_orders.txt", &head_o);
    load_orders("order_serving.txt",&head_s);
    load_orders("order_history.txt",&head_h);
    int choice;
    while (1) {
        mainmenu();
        scanf("%d", &choice);
        if (choice == 0) {
            break;
        }
        switch (choice) {
            case 1:
                clearScreen();
                clearBuffer();
                authenticate();
                break;
            case 2:
                clearScreen();
                customer();
                break;
            default:
                printf("Incorrect Choice! Please choose a valid option.\n");
                break;
        }
    }
    return 0;
}