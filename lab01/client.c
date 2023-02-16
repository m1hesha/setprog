#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

main(int argc, char *argv[]) {
  int sock, bytes_read;
  struct sockaddr_in servAddr, clientAddr;
  struct hostent *hp, *gethostbyname();
  socklen_t length;
  char *buf = malloc(sizeof(char) * 81);

  if (argc < 6) {
    printf("INSERT: IP PORT NumberOfMessages Delay Message\n");
    exit(1);
  }

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("ERROR: Can't get a socket\n");
    exit(1);
  }

  bzero((char *)&servAddr, sizeof(servAddr));
  hp = gethostbyname(argv[1]);
  bcopy(hp->h_addr, &servAddr.sin_addr, hp->h_length);
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));

  bzero((char *)&clientAddr, sizeof(clientAddr));
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddr.sin_port = 0;

  if (bind(sock, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
    perror("ERROR: Client didn't get a socket");
    exit(1);
  }
  printf("CLIENT: Ready to send.\n");

  memset(buf, 0, sizeof(char) * 81);
  for (int i = 0; i < atoi(argv[3]); i++) {
    strcpy(buf, argv[5]);
    printf("CLIENT: Message sent:\t  %s\n", buf);
    if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&servAddr,
               sizeof(servAddr)) < 0) {
      perror("ERROR: sendto() fail");
      exit(1);
    }

    if ((bytes_read = recvfrom(sock, buf, 81, 0, (struct sockaddr *)&servAddr,
                               &length)) < 0) {
      perror("ERROR: recvfrom() fail");
      exit(1);
    }
    printf("CLIENT: Message received: %s\n\n", buf);
    sleep(atoi(argv[4]));
  }
  close(sock);
  return 0;
}