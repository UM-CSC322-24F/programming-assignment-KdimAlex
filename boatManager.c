#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOATS 120
#define MAX_NAME_LENGTH 128
#define MAX_TAG_LENGTH 16

typedef enum {
    SLIP, LAND, TRAILOR, STORAGE
} BoatType;

typedef struct {
    int slipNumber;
    char bayLetter;
    char trailorTag[MAX_TAG_LENGTH];
    int storageNumber;
} BoatInfo;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int length;
    BoatType type;
    BoatInfo info;
    float amountOwed;
} Boat;

Boat* boats[MAX_BOATS];
int boatCount = 0;

void loadBoatsFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Boat *boat = (Boat*)malloc(sizeof(Boat));
        char typeStr[16];
        sscanf(line, "%[^,],%d,%[^,]", boat->name, &boat->length, typeStr);
        
        if (strcmp(typeStr, "slip") == 0) {
            boat->type = SLIP;
            sscanf(line, "%*[^,],%*d,%*[^,],%d,%f", &boat->info.slipNumber, &boat->amountOwed);
        } else if (strcmp(typeStr, "land") == 0) {
            boat->type = LAND;
            sscanf(line, "%*[^,],%*d,%*[^,],%c,%f", &boat->info.bayLetter, &boat->amountOwed);
        } else if (strcmp(typeStr, "trailor") == 0) {
            boat->type = TRAILOR;
            sscanf(line, "%*[^,],%*d,%*[^,],%[^,],%f", boat->info.trailorTag, &boat->amountOwed);
        } else if (strcmp(typeStr, "storage") == 0) {
            boat->type = STORAGE;
            sscanf(line, "%*[^,],%*d,%*[^,],%d,%f", &boat->info.storageNumber, &boat->amountOwed);
        }
        boats[boatCount++] = boat;
    }

    fclose(file);
}

void saveBoatsToFile(const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    for (int i = 0; i < boatCount; i++) {
        Boat *boat = boats[i];
        if (boat->type == SLIP) {
            fprintf(file, "%s,%d,slip,%d,%.2f\n", boat->name, boat->length, boat->info.slipNumber, boat->amountOwed);
        } else if (boat->type == LAND) {
            fprintf(file, "%s,%d,land,%c,%.2f\n", boat->name, boat->length, boat->info.bayLetter, boat->amountOwed);
        } else if (boat->type == TRAILOR) {
            fprintf(file, "%s,%d,trailor,%s,%.2f\n", boat->name, boat->length, boat->info.trailorTag, boat->amountOwed);
        } else if (boat->type == STORAGE) {
            fprintf(file, "%s,%d,storage,%d,%.2f\n", boat->name, boat->length, boat->info.storageNumber, boat->amountOwed);
        }
    }

    fclose(file);
}

void printInventory() {
    for (int i = 0; i < boatCount; i++) {
        Boat *boat = boats[i];
        if (boat->type == SLIP) {
            printf("%-20s %2d' slip    # %2d   Owes $%.2f\n", boat->name, boat->length, boat->info.slipNumber, boat->amountOwed);
        } else if (boat->type == LAND) {
            printf("%-20s %2d' land       %c   Owes $%.2f\n", boat->name, boat->length, boat->info.bayLetter, boat->amountOwed);
        } else if (boat->type == TRAILOR) {
            printf("%-20s %2d' trailor %s   Owes $%.2f\n", boat->name, boat->length, boat->info.trailorTag, boat->amountOwed);
        } else if (boat->type == STORAGE) {
            printf("%-20s %2d' storage # %2d   Owes $%.2f\n", boat->name, boat->length, boat->info.storageNumber, boat->amountOwed);
        }
    }
}

void addBoat() {
    if (boatCount >= MAX_BOATS) {
        printf("The marina is full.\n");
        return;
    }

    Boat *boat = (Boat*)malloc(sizeof(Boat));
    char typeStr[16];
    
    printf("Please enter the boat data in CSV format: ");
    char input[256];
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%[^,],%d,%[^,]", boat->name, &boat->length, typeStr);

    if (strcmp(typeStr, "slip") == 0) {
        boat->type = SLIP;
        sscanf(input, "%*[^,],%*d,%*[^,],%d,%f", &boat->info.slipNumber, &boat->amountOwed);
    } else if (strcmp(typeStr, "land") == 0) {
        boat->type = LAND;
        sscanf(input, "%*[^,],%*d,%*[^,],%c,%f", &boat->info.bayLetter, &boat->amountOwed);
    } else if (strcmp(typeStr, "trailor") == 0) {
        boat->type = TRAILOR;
        sscanf(input, "%*[^,],%*d,%*[^,],%[^,],%f", boat->info.trailorTag, &boat->amountOwed);
    } else if (strcmp(typeStr, "storage") == 0) {
        boat->type = STORAGE;
        sscanf(input, "%*[^,],%*d,%*[^,],%d,%f", &boat->info.storageNumber, &boat->amountOwed);
    }

    boats[boatCount++] = boat;
}

void removeBoat() {
    printf("Please enter the boat name: ");
    char name[MAX_NAME_LENGTH];
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < boatCount; i++) {
        if (strcasecmp(boats[i]->name, name) == 0) {
            free(boats[i]);
            for (int j = i; j < boatCount - 1; j++) {
                boats[j] = boats[j + 1];
            }
            boatCount--;
            printf("Boat removed.\n");
            return;
        }
    }
    printf("No boat with that name.\n");
}

void processPayment() {
    printf("Please enter the boat name: ");
    char name[MAX_NAME_LENGTH];
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    for (int i = 0; i < boatCount; i++) {
        if (strcasecmp(boats[i]->name, name) == 0) {
            float payment;
            printf("Please enter the amount to be paid: ");
            scanf("%f", &payment);
            getchar();

            if (payment > boats[i]->amountOwed) {
                printf("That is more than the amount owed, $%.2f\n", boats[i]->amountOwed);
                return;
            }
            boats[i]->amountOwed -= payment;
            printf("Payment processed.\n");
            return;
        }
    }
    printf("No boat with that name.\n");
}

void updateMonthlyCharges() {
    for (int i = 0; i < boatCount; i++) {
        if (boats[i]->type == SLIP) {
            boats[i]->amountOwed += 12.50 * boats[i]->length;
        } else if (boats[i]->type == LAND) {
            boats[i]->amountOwed += 14.00 * boats[i]->length;
        } else if (boats[i]->type == TRAILOR) {
            boats[i]->amountOwed += 25.00 * boats[i]->length;
        } else if (boats[i]->type == STORAGE) {
            boats[i]->amountOwed += 11.20 * boats[i]->length;
        }
    }
    printf("Monthly charges updated.\n");
}

int main() {
    const char *filename = "BoatData.csv";

    loadBoatsFromFile(filename);

    char option;
    do {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &option);
        getchar();

        option = tolower(option);

        switch (option) {
            case 'i':
                printInventory();
                break;
            case 'a':
                addBoat();
                break;
            case 'r':
                removeBoat();
                break;
            case 'p':
                processPayment();
                break;
            case 'm':
                updateMonthlyCharges();
                break;
            case 'x':
                saveBoatsToFile(filename);
                printf("Exiting the Boat Management System\n");
                break;
            default:
                printf("Invalid option %c\n", option);
        }
    } while (option != 'x');

    // Free allocated memory
    for (int i = 0; i < boatCount; i++) {
        free(boats[i]);
    }

    return 0;
}
