#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 *  Estrutura de mensagem
 */
typedef struct
{
    // Tipo da mensagem
    // (1= telemetria, 2=ACK, 3=equipe de drones , 4= concluso)
    uint16_t tipo;
    // tamanho do payload em bytes
    uint16_t tamanho;
} header_t;

/**
 * Estruturas de payload
 */

// Cliente → Servidor : Envio de telemetria (a cada 30s)
typedef struct
{
    int total;              // numero de cidades monitoradas
    telemetria_t dados[50]; // lista de (id_cidade, status)
} payload_telemetria_t;

typedef struct
{
    int id_cidade; // identificador do vertice
    int status;    // 0 = OK, 1 = ALERTA
} telemetria_t;

// Servidor → Cliente ou Cliente → Servidor : Confirma¸c˜ao de recebimento
typedef struct
{
    int status; // 0=ACK TELEMETRIA, 1=ACK EQUIPE DRONE, 2=ACK CONCLUSAO
} payload_ack_t;

// Servidor → Cliente : Ordem de envio de drone
typedef struct
{
    int id_cidade; // cidade onde o alerta foi detectado
    int id_equipe; // equipe de drones designada
} payload_equipe_drone_t;

// Cliente → Servidor : Conclus˜ao de miss˜ao
typedef struct
{
    int id_cidade; // cidade atendida
    int id_equipe; // equipe que atuou
} payload_conclusao_t;