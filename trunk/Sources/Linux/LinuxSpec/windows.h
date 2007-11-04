#ifndef WINDOWS_HH
#define WINDOWS_HH

//TODO:
//typedef unsigned char BYTE;
#define IN
#define OUT
#define HANDLE int

typedef union _ULARGE_INTEGER {
  struct {
    int LowPart;
    int HighPart;
  };
  struct {
    int LowPart;
    int HighPart;
  } u;
  unsigned long long QuadPart;
} ULARGE_INTEGER;

#endif
