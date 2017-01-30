#include <stdlib.h>
#include <stdio.h>
//#include <time.h>
#include <fcntl.h>

static int score[14];
static int d[14];

int random_dice();
void roll(int a[]);
void reroll(int a[]);
void display(const int a[]);
void Score(const int a[]);
int Lower(const int a[]);
int Upper(const int a[]);
int ThreeOfAKind(int b[]);
int FourOfAKind(int b[]);
int FullHouse(int b[]);
int SmallStraight(int b[]);
int LargeStraight(int b[]);
int Yahtzee(int b[]);
int Chance(int b[]);

unsigned char dice;

int main(int argc, const char * argv[]) {
    //srand((unsigned int)time(NULL));
    dice = open("/dev/dice", O_RDONLY);
    int a[5] = {0};
    //roll(a);
    while (d[6] < 13)
    {
        printf("---------------------\n");
        roll(a);
        display(a);
        int c;
        int e = 0;
        printf("\nWhich dice to reroll? ");
        setbuf(stdin, NULL);
        while ((c = getchar()) != '\n')
        {
            if (c == '0')
                e = 1;
            if (c != ' ')
                a[c-'1'] = 0;
        }
        if (e == 0)
        {
            reroll(a);
            display(a);
            printf("\nWhich dice to reroll? ");
            setbuf(stdin, NULL);
            while ((c = getchar()) != '\n')
            {
                if (c == '0')
                    e = 1;
                if (c != ' ')
                    a[c-'1'] = 0;
            }
            if (e == 0)
            {
                reroll(a);
                display(a);
            }
        }
        Score(a);
    }
    return 0;
}

int random_dice()
{
    unsigned char r;
    read(dice, &r, 1);
    r = r%6;
    return (int)r+1;
}

void roll(int a[])
{
    int i;
    for (i = 0; i < 5; i++)
        a[i] = random_dice();
}

void reroll(int a[])
{
    int i;
    for (i = 0; i < 5; i++)
    {
        if (a[i] == 0)
            a[i] = random_dice();
    }
}

void display(const int a[])
{
    int i;
    for (i = 0; i < 5; i++)
        printf("%d ", a[i]);
    printf("\n");
}

void DisplayScore(int n)
{
    if (d[n] == 0)
        return;
    else
        printf("%d", score[n]);
}

void Score(const int a[])
{
    int n;
    printf("\nPlace dice into:\n");
    printf("1) Upper Section\n");
    printf("2) Lower Section\n");
    printf("\nSelection? ");
    scanf("%d", &n);
    if (n == 1)
    {
        if (Upper(a) == 1)
        {
            Score(a);
            return;
        }
    }
    else if (n == 2)
    {
        if (Lower(a) == 1)
        {
            Score(a);
            return;
        }
    }
    int sum = 0;
    int i;
    for (i = 0; i < 6; i++)
        sum += score[i];
    if (score[6] == 0)
    {
        if (sum >= 63)
            score[6] = 35;
    }
    sum += score[6];
    for (i = 7; i < 14; i++)
        sum += score[i];
    printf("\nYour score so far is: %d\n", sum);
    printf("\nOnes:\t");DisplayScore(0);printf("\tTwos:\t");DisplayScore(1);
    printf("\nThrees:\t");DisplayScore(2);printf("\tFours:\t");DisplayScore(3);
    printf("\nFives:\t");DisplayScore(4);printf("\tSixes:\t");DisplayScore(5);
    printf("\nUpper Section Bonus: %d", score[6]);
    printf("\nThree of a Kind:");DisplayScore(7);printf("\tFour of a Kind:\t");DisplayScore(8);
    printf("\nSmall Straight:\t");DisplayScore(9);printf("\tLarge Straight:\t");DisplayScore(10);
    printf("\nFull House:\t");DisplayScore(11);printf("\tYahtzee:\t");DisplayScore(12);
    printf("\nChance:\t\t");DisplayScore(13);
    printf("\n\n");
}

int Upper(const int a[])
{
    int n;
    printf("\nPlace dice into:\n");
    printf("1) Ones\n");
    printf("2) Twos\n");
    printf("3) Threes\n");
    printf("4) Fours\n");
    printf("5) Fives\n");
    printf("6) Sixes\n");
    printf("\nSelection? ");
    scanf("%d", &n);
    if (d[n-1] != 0)
    {
        printf("This category has been assigned!\n");
        return 1;
    }
    int i;
    int sc = 0;
    for (i = 0; i < 5; i++)
    {
        if (a[i] == n)
            sc += n;
    }
    score[n-1] = sc;
    d[n-1] = 1;
    d[6]++;
    return 0;
}

int comp(const void *a, const void *b)
{
    return *(int *)a-*(int *)b;
}

int Lower(const int a[])
{
    int b[5];
    int i;
    for (i = 0; i < 5; i++)
        b[i] = a[i];
    qsort(b, 5, sizeof(int), comp);
    int n;
    printf("\nPlace dice into:\n");
    printf("1) Three of a Kind\n");
    printf("2) Four of a Kind\n");
    printf("3) Small Straight\n");
    printf("4) Large Straight\n");
    printf("5) Full House\n");
    printf("6) Yahtzee\n");
    printf("7) Chance\n");
    printf("\nSelection? ");
    scanf("%d", &n);
    if (d[n+6] != 0)
    {
        printf("This category has been assigned!\n");
        return 1;
    }
    int sc;
    switch (n)
    {
        case 1:
            sc = ThreeOfAKind(b);
            break;
        case 2:
            sc = FourOfAKind(b);
            break;
        case 3:
            sc = SmallStraight(b);
            break;
        case 4:
            sc = LargeStraight(b);
            break;
        case 5:
            sc = FullHouse(b);
            break;
        case 6:
            sc = Yahtzee(b);
            break;
        case 7:
            sc = Chance(b);
            break;
        default:
            break;
    }
    score[n+6] = sc;
    d[n+6] = 1;
    d[6]++;
    return 0;
}

int ThreeOfAKind(int b[])
{
    int score = b[0];
    int n = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        score += b[i+1];
        if (b[i] == b[i+1])
            n++;
    }
    if (n < 2)
        return 0;
    return score;
}

int FourOfAKind(int b[])
{
    int score = b[0];
    int n = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        score += b[i+1];
        if (b[i] == b[i+1])
            n++;
    }
    if (n < 3)
        return 0;
    return score;
}

int FullHouse(int b[])
{
    if (b[0] != b[1])
        return 0;
    if (b[1] != b[2])
    {
        if (b[2] != b[3])
            return 0;
        if (b[3] != b[4])
            return 0;
    }
    else
    {
        if (b[3] != b[4])
            return 0;
    }
    return 25;
}

int SmallStraight(int b[])
{
    int i;
    int n = 0;
    for (i = 0; i < 4; i++)
    {
        if (b[i] != b[i+1]-1)
            n++;
    }
    if (n > 1)
        return 0;
    else
        return 30;
}

int LargeStraight(int b[])
{
    int i;
    if (b[0] == 1)
    {
        for (i = 1; i < 5; i++)
        {
            if (b[i] == i+1)
                continue;
            else
                return 0;
        }
    }
    else if (b[0] == 2)
    {
        for (i = 1; i < 5; i++)
        {
            if (b[i] == i+2)
                continue;
            else
                return 0;
        }
    }
    else
        return 0;
    return 40;
}

int Yahtzee(int b[])
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (b[i] == b[i+1])
            continue;
        else
            return 0;
    }
    return 50;
}

int Chance(int b[])
{
    int score = 0;
    int i;
    for (i = 0; i < 5; i++)
        score += b[i];
    return score;
}
