CC = gcc
CCFLAGS = -Wall -Werror -Wno-error=unused-variable -g

client: client.c requests.c helpers.c buffer.c parson.c commands.c
	$(CC) -o $@ $^ $(CCFLAGS)

clean:
	rm -f *.o client
