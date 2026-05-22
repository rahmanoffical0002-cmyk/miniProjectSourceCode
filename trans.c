// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

#define COMMIT_COUNT 15

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
}; // end structure clientData

// prototypes
int initializeFile(const char *fileName);
unsigned int enterChoice(void);
unsigned int getAccountNumber(void);
void clearInput(void);
void textFile(FILE *readPtr);
void listAccounts(FILE *fPtr);
void viewAccount(FILE *fPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    printf("Repository commit count: %d\n", COMMIT_COUNT);
    cfPtr = fopen("credit.dat", "rb+");
    if (cfPtr == NULL)
    {
        if (!initializeFile("credit.dat"))
        {
            printf("%s: Unable to create data file.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        cfPtr = fopen("credit.dat", "rb+");
        if (cfPtr == NULL)
        {
            printf("%s: File could not be opened after initialization.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            listAccounts(cfPtr);
            break;
        case 6:
            viewAccount(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    } // end while

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// initialize data file with blank records
int initializeFile(const char *fileName)
{
    FILE *file = fopen(fileName, "wb+");
    if (file == NULL)
    {
        return 0;
    }

    struct clientData blankClient = {0, "", "", 0.0};
    for (unsigned int i = 0; i < 100; ++i)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, file);
    }

    fclose(file);
    return 1;
}

void clearInput(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

unsigned int getAccountNumber(void)
{
    unsigned int account;
    int result = scanf("%u", &account);
    if (result != 1)
    {
        clearInput();
        puts("Invalid input. Please enter a number from 1 to 100.");
        return 0;
    }

    if (account < 1 || account > 100)
    {
        puts("Account number must be between 1 and 100.");
        return 0;
    }

    return account;
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    double totalBalance = 0.0;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr); // sets pointer to beginning of file
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while ((result = fread(&client, sizeof(struct clientData), 1, readPtr)) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                    client.balance);
            totalBalance += client.balance;
        }
    }

    fprintf(writePtr, "\nTotal balance: %39.2f\n", totalBalance);
    fclose(writePtr);
    puts("accounts.txt created successfully.");
} // end function textFile

void listAccounts(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    int result;
    double totalBalance = 0.0;
    unsigned int count = 0;

    rewind(fPtr);
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    while ((result = fread(&client, sizeof(struct clientData), 1, fPtr)) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            totalBalance += client.balance;
            ++count;
        }
    }

    if (count == 0)
    {
        puts("No accounts found.");
    }
    else
    {
        printf("\nTotal accounts: %u, total balance: %.2f\n", count, totalBalance);
    }
}

void viewAccount(FILE *fPtr)
{
    unsigned int account;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number to view ( 1 - 100 ): ");
    account = getAccountNumber();
    if (account == 0)
    {
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
}

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update ( 1 - 100 ): ");
    account = getAccountNumber();
    if (account == 0)
    {
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);
    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        clearInput();
        puts("Invalid transaction amount.");
        return;
    }

    client.balance += transaction;
    printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

    fseek(fPtr, -((long)sizeof(struct clientData)), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
}

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                         // stores record read from file
    struct clientData blankClient = {0, "", "", 0.0}; // blank client
    unsigned int accountNum;                          // account number
    char confirm;

    printf("Enter account number to delete ( 1 - 100 ): ");
    accountNum = getAccountNumber();
    if (accountNum == 0)
    {
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u does not exist.\n", accountNum);
        return;
    }

    printf("Delete account #%u (%s %s, balance %.2f)? (y/n): ", accountNum, client.lastName, client.firstName,
           client.balance);
    clearInput();
    confirm = getchar();
    clearInput();
    if (confirm != 'y' && confirm != 'Y')
    {
        puts("Deletion cancelled.");
        return;
    }

    fseek(fPtr, -((long)sizeof(struct clientData)), SEEK_CUR);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    puts("Account deleted.");
}

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    printf("Enter new account number ( 1 - 100 ): ");
    accountNum = getAccountNumber();
    if (accountNum == 0)
    {
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) == 1 && client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        clearInput();
        puts("Invalid input. Record not created.");
        return;
    }

    client.acctNum = accountNum;
    fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    puts("New account created.");
}

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - list all existing accounts\n"
                 "6 - view an account\n"
                 "7 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInput();
        return 0;
    }

    return menuChoice;
} // end function enterChoice