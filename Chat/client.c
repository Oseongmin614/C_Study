#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int sock = 0;
char name[20];
volatile int flag = 0;

// 서버로부터 메시지를 받는 스레드
void *recv_msg(void *arg) {
    char buffer[BUFFER_SIZE];
    
    while (1) {
        int receive = recv(sock, buffer, BUFFER_SIZE, 0);
        
        if (receive <= 0) {
            flag = 1;
            break;
        }
        
        buffer[receive] = '\0';
        printf("%s\n", buffer);
    }
    
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    pthread_t recv_thread;
    
    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // IP 주소 변환
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    
    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }
    
    printf("채팅 서버에 연결되었습니다.\n");
    
    // 메시지 수신 스레드 생성
    if (pthread_create(&recv_thread, NULL, &recv_msg, NULL) != 0) {
        perror("pthread_create error");
        exit(EXIT_FAILURE);
    }
    
    // 사용자 입력 처리
    char message[BUFFER_SIZE];
    while (1) {
        if (flag) {
            printf("서버와 연결이 끊어졌습니다.\n");
            break;
        }
        
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';
        
        if (strcmp(message, "exit") == 0) {
            break;
        }
        
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("send failed");
            break;
        }
    }
    
    close(sock);
    return 0;
}
