#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 81

int main() {
  int sockMain, msgLength;
  socklen_t addrLength;
  struct sockaddr_in servAddr, clientAddr;
  char *buf = malloc(sizeof(char) * BUFLEN);

  if ((sockMain = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("ERROR: Server can't open socket for UDP");
    exit(1);
  }

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = 0;

  if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
    perror("ERROR: bind() fail");
    exit(1);
  }

  addrLength = sizeof(servAddr);
  if (getsockname(sockMain, (struct sockaddr *)&servAddr, &addrLength) < 0) {
    perror("ERROR: getsockname() fail");
    exit(1);
  }

  printf("SERVER: Port number -\t %d\n", ntohs(servAddr.sin_port));
  int i = 1;
  while (1) {
    addrLength = sizeof(clientAddr);
    bzero(buf, BUFLEN);
    if ((msgLength = recvfrom(sockMain, buf, BUFLEN, 0,
                              (struct sockaddr *)&clientAddr, &addrLength)) <
        0) {
      perror("ERROR: Bad client socket");
      exit(1);
    }

    printf("SERVER: Client IP:\t %s\n", inet_ntoa(clientAddr.sin_addr));
    printf("SERVER: Cleint PORT:\t %d\n", ntohs(clientAddr.sin_port));
    printf("SERVER: Message length - %d\n", msgLength);
    printf("SERVER: Client message:\t %s\n", buf);

    if (i % 2 == 0) {
      buf = strcat(buf, " even");
    } else {
      buf = strcat(buf, " odd");
    }

    if (sendto(sockMain, buf, BUFLEN, 0, (struct sockaddr *)&clientAddr,
               sizeof(clientAddr)) < 0) {
      perror("ERROR: sendto() fail");
      exit(1);
    }
    printf("SERVER: Changed message: %s\n\n", buf);
    i++;
  }
  close(sockMain);
  return 0;
}