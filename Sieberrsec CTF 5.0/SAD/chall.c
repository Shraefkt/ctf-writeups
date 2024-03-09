#include <stdio.h>
#include <stdlib.h>
char agents[][8] = {"Alice","Bob","Craig","David","Eve","Faythe"};
void menu() {
    puts("--- MENU ---");
    puts("1. Read a name");
    puts("2. Change a name");
    puts("3. Exit");
    printf("> ");
}
int main()
{
    puts("Welcome to the secret agent database (sad)!!!");
    puts("Someone told me it can be hacked with ease...");
    int choice;
    int index;
    while (1) {
        menu();
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                printf("Agent number: ");
                scanf("%d", &index);
                if (index > (int)(sizeof(agents)/8)-1) {
                    puts("tsk tsk don't be naughty...");
                    exit(1);
                }
                printf("[Agent no. %d]: %s\n",index, agents[index]);
                break;
            case 2:
                printf("Agent number: ");
                scanf("%d", &index);
                if (index > (int)(sizeof(agents)/8)-1) {
                    puts("tsk tsk don't be naughty...");
                    exit(1);
                }
                getchar();
                printf("New name: ");
                fgets(agents[index],8,stdin);
                printf("[Agent no. %d]: %s\n",index, agents[index]);
                break;
            case 3:
                puts("Bye");
                exit(1);
            default:
                puts("Invalid Choice");
        }
    }
    return 1;
}
//gcc  ./chall.c -Wl,-z,norelro -o sad