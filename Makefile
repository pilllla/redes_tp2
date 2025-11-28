# Definicoes de Variaveis
CC = gcc
CFLAGS = -Wall -std=c17 -g
RM = rm -f

# Objetos necessarios para o executavel 'client'
CLIENT_OBJS = client.o utils.o

# Objetos necessarios para o executavel 'server'
SERVER_OBJS = server.o utils.o

# Alvo default: constroi tanto o cliente quanto o servidor
all: client server

# Regra para construir o client
client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o client

# Regra para construir o server
server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o server

# Regra generica para compilar arquivos .c em .o
# O $< representa o arquivo de dependencia (.c) e $@ representa o alvo (.o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Adiciona a dependencia do utils.o em utils.h
utils.o: utils.c utils.h

# Alvo de limpeza: remove todos os arquivos objeto e executaveis gerados
clean:
	$(RM) $(CLIENT_OBJS) $(SERVER_OBJS) client server