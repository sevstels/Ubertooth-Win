//==============================================================================
//File name:    "win-pipe.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include <windows.h>
#include "win-pipe.h"
#include <iostream>
#include <conio.h> 

// pipe handle
static HANDLE hPipe = NULL; 

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
static void print_last_error(void)
{
  LPCSTR lpMsgBuf;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	            FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, GetLastError(), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) & lpMsgBuf, 0, NULL);

  //MessageBox(NULL, lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION);
  //LocalFree(lpMsgBuf);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
static int named_pipe_create(void)
{
  hPipe = CreateNamedPipe("\\\\.\\pipe\\wireshark",
        PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1, 65536, 65536, 300, NULL);
    
  if(hPipe == INVALID_HANDLE_VALUE)
  {
    print_last_error();
    return -1;
  }

 BOOL result = ConnectNamedPipe(hPipe, NULL);
 return (int) result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
DWORD data_write(const void *ptr, size_t size, size_t nitems)
{
  DWORD cbWritten = 0;

  if(hPipe)
  {
    (void)WriteFile(hPipe, ptr, size*nitems, &cbWritten, NULL);
  }

  return cbWritten;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
void cleanup(void)
{
 if(hPipe)
 {
   FlushFileBuffers(hPipe);
   DisconnectNamedPipe(hPipe);
   CloseHandle(hPipe);
 }
}

//------------------------------------------------------------------------------
// tshark -l -n -r "\\.\pipe\tsharkpipe2" wireshark -ni \\.\pipe\wireshark -k
//------------------------------------------------------------------------------ 
int test_pipe(void)
{
  FILE *pFile = NULL;
  char *pFilename = {"test.pcap"};
  char buffer[512] = {""};
  size_t num_read = 0;

  pFile = fopen(pFilename, "rb");
  if(pFile==NULL) return -1; 
 
  int result = named_pipe_create();
  if(!result) return -1;

  while((num_read = fread(buffer,1,sizeof(buffer),pFile)) == sizeof(buffer)) 
   {
     data_write(buffer, 1, num_read);
   }
        
  //write remaining stuff
  if(num_read)
   {
     data_write(buffer, 1, num_read);
   }
  
  fclose(pFile);
    
  cleanup();
  return 0;
}
