// ================================================ ============================
// Name: httpGet.c
// Author: bus systems
// Version: SuSe20
// ================================================ ============================


// Query an HTTP URL and output the results on the screen.
// For MS compilers: The linker file wsock32.lib must be in the project settings
// be entered!

// update WS09 / 10: This example runs on Win and Unix.

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else // Here are the replacements for the BSD socks.
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <windows.h>
#define  SOCKADDR_IN struct sockaddr_in
#define  SOCKADDR    struct sockaddr
#define  HOSTENT     struct hostent
#define  SOCKET      int
int WSAGetLastError() { return errno; }
int closesocket(int s) { return close(s); }
#endif
#include <stdio.h>
#include <stdlib.h>


void perr_exit(char* msg, int ret_code)
{
  printf("%s, Error: ",msg);
  printf("%d\n",ret_code);
  exit(ret_code);
}


int main(int argc, char** argv)
{
  char* site;
  char* host;
  char  img_address[50] = "data/hska-logo.jpg";
  //char  *img_address = "";
  char  url[1024];
  char  send_buf[256];
  char  recv_buf[256];
  long  rc;
  //CURLcode res;
  FILE *pfile = fopen("C:\\Users\\sonus\\eclipse-workspace\\httpGet\\data\\img.jpeg", "a");
  SOCKET s;
  SOCKADDR_IN addr;
#ifdef _WIN32
  WSADATA wsa;
#endif
  HOSTENT* hent;

  // Before you can start, you have to call this WSAStartup function.
  // Initializes TCPIP stack.
#ifdef _WIN32
  if(WSAStartup(MAKEWORD(2,0),&wsa))
    perr_exit("WSAStartup failed",WSAGetLastError());
#endif

  addr.sin_family=AF_INET;
  addr.sin_port=htons(80);

  printf("\nURL: ");
  scanf("%s",url);

  if(strncmp("http://",url,7)==0)
    host=url+7;
  else
    host=url;

  if((site=strchr(host,'/'))!=0)
    *site++='\0';
  else
    site=host+strlen(host); /* \0 */

  printf("\nHost: %s\n",host);
  printf("Site: %s\n",site);
  printf("Connecting....\n");

  if((addr.sin_addr.s_addr=inet_addr((const char*)host))==INADDR_NONE)
  {
    if(!(hent=gethostbyname(host)))
      perr_exit("Cannot resolve Host",WSAGetLastError());

    strncpy((char*)&addr.sin_addr.s_addr,hent->h_addr,4);

    if(addr.sin_addr.s_addr==INADDR_NONE)
      perr_exit("Cannot resolve Host",WSAGetLastError());
  }

  s=socket(AF_INET,SOCK_STREAM,0);
#ifdef _WIN32
  if (s==INVALID_SOCKET)
#else
  if (s<0)
#endif
    perr_exit("Cannot create Socket",WSAGetLastError());

  if( connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR)))
    perr_exit("Cannot connect",WSAGetLastError());

  printf("Connected to %s...\n",host);

  // Prepare the HTTP Get Header
  sprintf(send_buf,"GET /%s HTTP/1.1\r\n"
		  "Host: %s\r\n\r\n",img_address,host);

  printf("Command sent to server: \n%s\n",send_buf);

  if((send(s,&send_buf,strlen(send_buf),0))<strlen(send_buf))
    perr_exit("Cannot send Data",WSAGetLastError());

  printf("----Result----\n");

  while((rc=recv(s,recv_buf,255,0))>0)
  {
    printf("%s",recv_buf);
    //Append the downloaded data into a jpeg file
    fwrite(recv_buf, 1, rc, pfile);
  }

  printf("\n---------------------------------------------------\n");
  // close the socket
  closesocket(s);
  // close file stream
  fclose(pfile);
  printf("Deleting irrelevant lines.....\n");
  system("sed -i '1,9d' 'C:\\Users\\sonus\\eclipse-workspace\\httpGet\\data\\img.jpeg'");
  printf("\nDONE!");
  return 0;
}
