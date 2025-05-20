#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    int id;
    char name[20];
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;

// 모든 클라이언트에게 메시지 전송
void broadcast_message(char *message, int sender_id) {
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
           
            char formatted_msg[BUFFER_SIZE + 30];
            sprintf(formatted_msg, "client%d: %s", sender_id, message);
            
            if (send(clients[i]->socket, formatted_msg, strlen(formatted_msg), 0) < 0) {
                perror("send failed");
                break;
            }
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

// 클라이언트 추가
void add_client(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    
    client_count++;
    pthread_mutex_unlock(&clients_mutex);
}

// 클라이언트 제거
void remove_client(int id) {
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->id == id) {
            free(clients[i]);
            clients[i] = NULL;
            break;
        }
    }
    
    client_count--;
    pthread_mutex_unlock(&clients_mutex);
}

// 클라이언트 처리 스레드
void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char welcome[BUFFER_SIZE];
    int leave_flag = 0;
    
    client_t *cli = (client_t *)arg;
    

    sprintf(welcome, "client%d님이 입장했습니다.", cli->id);
    broadcast_message(welcome, 0); 
    
    while (1) {
        int receive = recv(cli->socket, buffer, BUFFER_SIZE, 0);
        
        if (receive <= 0) {
            sprintf(buffer, "client%d님이 퇴장했습니다.", cli->id);
            broadcast_message(buffer, 0);
            leave_flag = 1;
            break;
        } else {
            buffer[receive] = '\0';
            broadcast_message(buffer, cli->id);
        }
    }
    
    close(cli->socket);
    remove_client(cli->id);
    pthread_detach(pthread_self());
    
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    
    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // 포트 재사용 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // 연결 대기
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("채팅 서버가 %d 포트에서 실행 중입니다...\n", PORT);
    
    int client_id = 1;
    
    while (1) {
        // 클라이언트 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("accept");
            continue;
        }
        
        // 최대 클라이언트 수 확인
        if (client_count >= MAX_CLIENTS) {
            printf("최대 클라이언트 수에 도달했습니다. 연결을 거부합니다.\n");
            close(new_socket);
            continue;
        }
        
        // 클라이언트 구조체 생성
        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->socket = new_socket;
        client->id = client_id++;
        sprintf(client->name, "client%d", client->id);
        
        // 클라이언트 추가
        add_client(client);
        
        // 클라이언트 처리 스레드 생성
        pthread_t tid;
        if (pthread_create(&tid, NULL, &handle_client, (void *)client) != 0) {
            perror("pthread_create");
            close(new_socket);
            free(client);
            continue;
        }
        
        // 스레드 분리
        pthread_detach(tid);
        
        printf("새로운 클라이언트가 연결되었습니다. ID: %d\n", client->id);
    }
    
    close(server_fd);
    return 0;
}
