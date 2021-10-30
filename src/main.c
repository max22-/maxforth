#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define mf_printf printf
#define mf_read read
#define mf_fprintf fprintf
#define mf_malloc malloc
#define mf_free free

#define STACK_SIZE 256
#define MEMORY_SIZE 65536
typedef long cell_t;
cell_t *stack;
void *memory, *here, *latest;
unsigned int sp = 0;

char word[256];
unsigned char word_len=0;

unsigned int base = 10;
enum {IMMEDIATE, COMPILE} state = IMMEDIATE;

#define F_IMMED 0x80
#define F_HIDDEN 0x20
#define F_LENMASK 0x1f

int is_sep(char c)
{
  return c == ' ' || c == '\n' || c == '\r';
}

static int get_next()
{
  ssize_t result;
  char c;
  word_len = 0;
  while(1) {
    result = mf_read(STDIN_FILENO, &c, 1);
    if(result <= 0)
      return 0;
    if(is_sep(c) && word_len > 0) {
      word[word_len] = 0;
      return 1;
    }
    else
      word[word_len++] = c;
    if(word_len == 255) {
      mf_fprintf(stderr, "Error : word too long\n");
      word_len = 0;
      return 1;
    }
  }
}

int push(cell_t c)
{
  if(sp >= STACK_SIZE - 1) {
    mf_fprintf(stderr, "Error : stack overflow\n");
    return 0;
  }
  stack[sp++] = c;
  return 0;
}

cell_t pop()
{
  if(sp == 0) {
    mf_fprintf(stderr, "Error : stack underflow\n");
    return 0;
  }
  return stack[--sp];
}

void print_stack()
{
  int i;
  mf_printf("<%d> ", sp);
  for(i = 0; i < sp; i++)
    mf_printf("%ld ", stack[i]);
  mf_printf("\n");
}

void dot()
{
  mf_printf("%ld ", pop());
}

void add()
{
  cell_t a, b;
  b = pop();
  a = pop();
  push(a + b);
}

void sub()
{
  cell_t a, b;
  b = pop();
  a = pop();
  push(a - b);
}

void mul()
{
  cell_t a, b;
  b = pop();
  a = pop();
  push(a * b);
}

void divide()
{
  cell_t a, b;
  b = pop();
  a = pop();
  push(a / b);
}

void emit()
{
  char c = pop();
  write(STDOUT_FILENO, &c, 1);
}

void _here()
{
  push((cell_t)here);
}

void lbrac()
{
  state = IMMEDIATE;
}

void rbrac()
{
  state = COMPILE;
}

void create()
{
  unsigned char len = pop();
  char* name = (char*)pop();
  *((cell_t*)here) = (cell_t)latest;
  here += sizeof(cell_t);
  *((char*)here) = len;
  here += 1;
  memcpy(here, name, len);
  here += len;
}

void *find(char *name, int len)
{
  if(!strcmp(name, ".s"))
    return print_stack;
  else if(!strcmp(name, "."))
    return dot;
  else if(!strcmp(name, "+"))
    return add;
  else if(!strcmp(name, "-"))
    return sub;
  else if(!strcmp(name, "*"))
    return mul;
  else if(!strcmp(name, "/"))
    return divide;
  else if(!strcmp(name, "emit"))
    return emit;
  else if(!strcmp(name, "create"))
    return create;
  else if(!strcmp(name, "here"))
    return _here;
  return NULL;
}



int main(int argc, char *argv[])
{
  void *word_addr;
  char *end_ptr;
  long l;
  
  mf_printf("maxForth\n");
  stack = (cell_t*)mf_malloc(sizeof(cell_t) * STACK_SIZE);
  if(stack == NULL) {
    mf_fprintf(stderr, "Not enough memory\n");
    return 1;
  }
  mf_printf("cell_size : %ld bytes\n", sizeof(cell_t));
  mf_printf("stack size : %d cells\n", STACK_SIZE);
  memory = mf_malloc(MEMORY_SIZE);
  here = memory;
  latest = NULL;
  if(memory == NULL) {
    mf_fprintf(stderr, "Not enough memory\n");
    return 1;
  }
  mf_printf("memory : %d bytes\n", MEMORY_SIZE);
  
  while(get_next()) {
    word_addr = find(word, word_len);
    if(word_addr == NULL) {
      l = strtol(word, &end_ptr, base);
      if(end_ptr - word == word_len)
	push(l);
      else
	mf_fprintf(stderr, "Error : undefined word\n");
    }
    else ((void (*)(void))word_addr)();
    print_stack();
  }
  
  return 0;
}
