#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.h"

// ADDR 127.0.0.1
#define PORT 8080
#define BUF_SIZE 2048

// bool city_alert[50];
int capital_dispatched[50];

typedef struct
{
    int id;
    int type;
    char name[100];
} node_t;

typedef struct
{

} city_alert_t;

void fill_graph(int N, int M, node_t *g, int graph[N][N], bool include_edges)
{
    FILE *fptr;

    fptr = fopen("grafo_amazonia_legal.txt", "r");
    fscanf(fptr, "%d %d", &N, &M);

    for (int i = 0; i < N; i++)
    {
        fscanf(fptr, "%d %s %d", &g[i].id, g[i].name, &g[i].type);
    }

    int v1, v2, weight;

    // grafo é não direcionado - (1, 2, 8) e (2, 1, 8) são arestas válidas
    if (include_edges)
    {
        for (int i = 0; i < M; i++)
        {
            fscanf(fptr, "%d %d %d", &v1, &v2, &weight);
            graph[v1][v2] = weight;
            graph[v2][v1] = weight;
        }
    }

    fclose(fptr);

    for (int i = 0; i < 50; i++)
    {
        city_alert[i] = 0;
    }

    for (int i = 0; i < 50; i++)
    {
        capital_dispatched[i] = 0;
    }
}

int min_distance(int N, int dist[], bool shortest_set[])
{
    // Initialize min value
    int min = INT_MAX;
    int min_index;

    for (int v = 0; v < N; v++)
    {
        if (shortest_set[v] == false && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

void dispatch_capital(int c)
{
    capital_dispatched[c] = 1;
}

int dijkstra(int N, int M, int graph[N][N], node_t *g, int src)
{
    int dist[N];

    bool shortest_set[N];
    // shortest_set[i] will be true if vertex i is
    // included in shortest
    // path tree or shortest distance from src to i is
    // finalized

    // Initialize all distances as INFINITE and stpSet[] as
    // false
    for (int i = 0; i < N; i++)
    {
        dist[i] = INT_MAX;
        shortest_set[i] = false;
    }

    dist[src] = 0;

    for (int count = 0; count < N - 1; count++)
    {
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed
        int u = min_distance(N, dist, shortest_set);

        // Mark the picked vertex as processed
        shortest_set[u] = true;

        // Update dist value
        for (int v = 0; v < N; v++)
        {
            // Update dist[v] only if is not in shortest_set,
            // there is an edge from u to v, and total
            // weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!shortest_set[v] && graph[u][v] > 0 && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    int min_dist = INT_MAX;
    int id_equipe_selecionada = -1; // -1 indica que não achou ninguém

    for (int i = 0; i < N; i++)
    {
        if ((g[i].type == 1) && (capital_dispatched[i] == 0))
        {
            if (dist[i] < min_dist)
            {
                min_dist = dist[i];
                id_equipe_selecionada = i;
            }
        }
    }

    return id_equipe_selecionada;
}

int get_my_addr(const char *addr, struct sockaddr_storage *storage)
{
    memset(addr, 0, sizeof(*addr));

    if (strcmp(addr, "v4") == 0)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;

        ipv4->sin_family = AF_INET;
        ipv4->sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &ipv4->sin_addr) <= 0)
        {
            perror("Erro ao converter IP v4");
            return -1;
        }
        return 0;
    }

    else if (strcmp(addr, "v6") == 0)
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr;

        ipv6->sin6_family = AF_INET6;
        ipv6->sin6_port = htons(PORT);

        if (inet_pton(AF_INET6, "::1", &ipv6->sin6_addr) <= 0)
        {
            perror("Erro ao converter IP v6");
            return -1;
        }
        return 0;
    }

    return -1;
}

void send_ack(uint16_t prev_type, int sockfd, struct sockaddr_storage *dest_addr, socklen_t addr_len)
{
    header_t h;
    h.tipo = htons(2);
    h.tamanho = htons(sizeof(payload_ack_t));

    payload_ack_t payload_ack;

    if (prev_type == 1)
    {
        payload_ack.status = htons(0);
    }
    else if (prev_type == 4)
    {
        payload_ack.status = htons(2);
    }

    char buffer[sizeof(header_t) + sizeof(payload_ack_t)];

    memcpy(buffer, &h, sizeof(header_t));
    memcpy(buffer + sizeof(header_t), &payload_ack, sizeof(payload_ack_t));

    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)dest_addr, addr_len);
}

void send_equipe_drone(int id_cap, int id_city, int sockfd, struct sockaddr_storage *dest_addr, socklen_t addr_len)
{
    header_t h;
    h.tipo = htons(3);
    h.tamanho = htons(sizeof(payload_equipe_drone_t));

    payload_equipe_drone_t payload_drone;

    payload_drone.id_cidade = htonl(id_city);
    payload_drone.id_equipe = htonl(id_cap);

    char buffer[sizeof(header_t) + sizeof(payload_equipe_drone_t)];

    memcpy(buffer, &h, sizeof(header_t));
    memcpy(buffer + sizeof(header_t), &payload_drone, sizeof(payload_equipe_drone_t));

    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)dest_addr, addr_len);
}

void process_dtgr(char *buffer, int sockfd, struct sockaddr_storage *dest_addr, socklen_t addr_len, int N, int M, int graph[N][N], node_t *g, int src)
{
    header_t *h = (header_t *)buffer;
    uint16_t type = ntohs(h->tipo);
    uint16_t tamanho = h->tamanho;

    // pula o tamanho do header
    char *ptr_payload = buffer + sizeof(header_t);

    switch (type)
    {
    case 1:
        payload_telemetria_t *payload = (payload_telemetria_t *)ptr_payload;

        int total_cities = ntohl(payload->total);
        bool alert_found = false;

        for (int i = 0; i < total_cities; i++)
        {
            if (ntohl(payload->dados[i].status) == 1)
            {
                int id_alert = ntohl(payload->dados[i].id_cidade);
                city_alert[id_alert] = true;

                alert_found = true;
            }
        }

        send_ack(type, sockfd, dest_addr, addr_len);

        if (alert_found == true)
        {
            for (int i = 0; i < 50; i++)
            {
                if (city_alert[i] == true)
                {
                    int capital = dijkstra(N, M, graph, g, i);
                }
            }
        }

        break;

    case 4:

        break;
    default:

        send_ack(type, sockfd, dest_addr, addr_len);

        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <v4|v6>\n", argv[0]);
        return 1;
    }

    // inicializar o grafo em memória

    int N = 0;
    int M = 0;

    FILE *fptr;
    fptr = fopen("grafo_amazonia_legal.txt", "r");

    if (fptr == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    fscanf(fptr, "%d %d", &N, &M);
    fclose(fptr);

    node_t *v = (node_t *)malloc(N * sizeof(node_t));

    int graph[N][N];

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            graph[i][j] = 0;
        }
    }

    fill_graph(N, M, v, graph, true);

    // conectividade

    int sockfd;
    struct sockaddr_storage my_addr;

    if (get_my_addr(argv[1], &my_addr) == 0)
    {
        printf("Endereço configurado com sucesso para %s!\n", argv[1]);
    }

    if ((sockfd = socket(my_addr.ss_family, SOCK_DGRAM, 0)) == -1)
    {
        perror("Erro no socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
    {
        perror("Erro no bind");
        close(sockfd);
        return 1;
    }

    while (true)
    {
        char buffer[BUF_SIZE];
        struct sockaddr_storage client_addr;

        socklen_t len_c;
        int n;
        len_c = sizeof(client_addr);

        n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &len_c);

        if (n > 0)
        {
            process_dtgr(buffer);
        }
    }

    // fim
    free(v);
    return 0;
}
