#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#define MAX_MEM 1<<30

static void *base = NULL;
void dump_heap();
void split(int n);

struct Block
{
  char head;
  struct Block *prev;
  struct Block *next;
};

static struct Block *freelist[26];

void *my_buddy_malloc(int size)
{
  if (base == NULL)
  {
    base = mmap(NULL, MAX_MEM, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
    int i;
    for (i = 0; i < 26; i++)
      freelist[i] = NULL;
    freelist[25] = (struct Block*)base;
    freelist[25]->head = 0;
    freelist[25]->head = freelist[25]->head|30;
    freelist[25]->prev = NULL;
    freelist[25]->next = NULL;
  }
//  dump_heap();

  int n = 0;
  int cur = 32;
  while (cur <= size)
  {
    n++;
    cur*=2;
  }
  if (n >= 26)
    return NULL;
  split(n);
  //dump_heap();
  freelist[n]->head = freelist[n]->head|128;
  struct Block *p = freelist[n];
  //p->head = p->head|128;
  freelist[n] = p->next;
  if (p->next != NULL)
  {
    freelist[n]->prev = NULL;
  }
  //printf("%d\n", p->head);
  return ((char*)p+1);
}

void split(int n)
{
    int index = n;
    while (freelist[index] == NULL)
      index++;
    while (index != n)
    {
      struct Block *temp1 = freelist[index];
      freelist[index] = temp1->next;
      if (temp1->next != NULL)
        freelist[index]->prev = NULL;
      char power = temp1->head;
      int new = (1<<power)/2;
      struct Block *temp2 = (struct Block*)(((char*)temp1)+new);
      struct Block block1;
      struct Block block2;
      block1.head = 0;
      block1.head = block1.head|(index+4);
      block2.head = 0;
      block2.head = block2.head|(index+4);
      block1.next = temp2;
      block1.prev = NULL;
      block2.next = NULL;
      block2.prev = temp1;
      *temp1 = block1;
      *temp2 = block2;
      if (freelist[index-1] == NULL)
        freelist[index-1] = temp1;
      else
      {
        temp2->next = freelist[index-1];
        freelist[index-1]->prev = temp2;
        freelist[index-1] = temp1;
      }
      index--;
    }
}
void *my_free(void *ptr)
{
  char *p = ptr;
  p-=1;
  p[0] = p[0]&127;
  char blocksize = p[0];
  int index = (int)blocksize-5;
  int addr = p-(char*)base;
  int size = 1<<blocksize;
  int offset = addr^size;
  char *buddy = (char*)base+offset;
  char buddyhead = buddy[0];
  int bit = buddyhead>>7&1;
  struct Block *block = (struct Block*)p;
  struct Block f;
  f.head = blocksize;
  f.next = NULL;
  f.prev = NULL;
  *block = f;
  while (bit == 0)
  {
    struct Block head;
    head.head = blocksize+1;
    head.prev = NULL;
    head.next = NULL;
    *block = head;
    struct Block *buddy2 = (struct Block*)buddy;
    if (buddy2->prev == NULL)
      freelist[index] = buddy2->next;
    else
      buddy2->prev->next = buddy2->next;
    if (buddy < p)
    {
      p = buddy;
      block = (struct Block*)p;
      *block = head;
    }
    blocksize = blocksize+1;
    index = (int)blocksize-5;
    if (index == 25)
      break;
    size = 1<<blocksize;
    offset = addr^size;
    buddy = (char*)base+offset;
    buddyhead = buddy[0];
    bit = buddyhead>>7&1;
    //printf("%d\n", bit);
  }
  int n = (int)p[0]-5;
  if (freelist[n] != NULL)
  {
    freelist[n]->prev = block;
    freelist[n] = block;
  }
  else
    freelist[n] = block;
}

void dump_heap()
{
    int i;
    for (i = 0; i < 26; i++)
    {
      if (freelist[i] == NULL)
        printf("%d->NULL\n", i+5);
      else
        printf("%d->[%d:%d:%d]\n", i+5, (freelist[i]->head)>>7&1, (void*)freelist[i]-base, 1<<(freelist[i]->head&127));
    }
}
/*
int main()
{
  char *p = my_buddy_malloc(8000);
  dump_heap();
  char *q = my_buddy_malloc(100000);
  dump_heap();
  my_free(p);
  dump_heap();
  my_free(q);
  dump_heap();
  return 0;
}
*/
