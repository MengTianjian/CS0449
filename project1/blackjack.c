//
//  main.c
//  CS0449Project1Blackjack
//
//  Created by Meng Tianjian on 9/18/16.
//  Copyright © 2016 Pitt. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct
{
    int value;
    int total;
    int acer;
}Player;

void GenerateValue(Player *player);
void PrintValue(Player player);
void Init(Player *a);

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    Player dealer, you;
    Init(&dealer);
    Init(&you);
    GenerateValue(&dealer);
    printf("The dealer:\n%d + ?\n\n", dealer.value);
    GenerateValue(&you);
    GenerateValue(&you);
    printf("You:\n");
    PrintValue(you);
    char YourInput[6];
    printf("\n\nWould you like to \"hit\" or \"stand\"?");
    scanf("%s", YourInput);
    while (strcmp("hit", YourInput) == 0)
    {
        printf("--------------------\n");
        printf("The dealer:\n%d + ?\n\n", dealer.total);
        GenerateValue(&you);
        printf("You:\n");
        PrintValue(you);
        if (you.total > 21)
        {
            printf(" BUSTED!\n\nYou busted. Dealer wins.\n\n");
            return 0;
        }
        else
        {
            printf("\n\nWould you like to \"hit\" or \"stand\"?");
            scanf("%s", YourInput);
        }
    }
    while (dealer.total < 17)
    {
        printf("--------------------\n");
        printf("You:\n");
        PrintValue(you);
        GenerateValue(&dealer);
        printf("\n\nDealer:\n");
        PrintValue(dealer);
        if (dealer.total > 21)
        {
            printf(" BUSTED!\n\nDealer busted. You win.\n\n");
            return 0;
        }
        else if (dealer.total >= 17)
            printf("\n\nDealer stand.\n\n");
        else
            printf("\n\nDealer hit.\n");
    }
    if (dealer.total > you.total)
        printf("Dealer wins.\n\n");
    else if (you.total > dealer.total)
        printf("You win.\n\n");
    else
        printf("Draw.\n\n");
    return 0;
}

void Init(Player *player)
{
    player->value = 0;
    player->total = 0;
    player->acer = 0;
}

void PrintValue(Player player)
{
    printf("%d + %d = %d", player.total - player.value, player.value, player.total);
}

void GenerateValue(Player *player)
{
    int tempvalue = rand() % 13 + 1;
    player->value = rand() % 13 + 1;
    if (tempvalue == 11 || tempvalue == 12 || tempvalue == 13)
        tempvalue = 10;
    if (tempvalue == 1)
    {
        if (player->total + 11 <= 21 && player->acer == 0)
        {
            player->acer = 1;
            tempvalue = 11;
        }
        else
            tempvalue = 1;
    }
    player->value = tempvalue;
    player->total += player->value;
    if (player->total > 21 && player->acer == 1)
    {
        player->total -= 10;
    }
}