#include "utils.h"

void init_payload_telemetria(payload_telemetria_t *telemetria)
{
    for (int i = 0; i < 50; i++)
    {
        telemetria->dados[i].id_cidade = i;
        telemetria->dados[i].status = 0;
    }
}