#include <stdio.h>
#include <unistd.h>

char word[256];
unsigned char word_len=0;

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
    result = read(STDIN_FILENO, &c, 1);
    if(result <= 0)
      return 0;
    if(is_sep(c) && word_len > 0) {
      word[word_len] = 0;
      return 1;
    }
    else
      word[word_len++] = c;
    if(word_len == 255) {
      fprintf(stderr, "Error : word too long\n");
      word_len = 0;
      return 1;
    }
  }
}

int main(int argc, char *argv[])
{
  printf("maxForth\n");
  while(get_next()) {
    printf("%s\n", word);
  }
  return 0;
}
