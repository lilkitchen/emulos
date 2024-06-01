/*
 * main.c
 */

#ifdef __WIN32__
#include <io.h>
#include <fcntl.h>
#endif

#include <locale.h>
#include <malloc.h>
#include <stdio.h>
#include <readline/readline.h>

#include "emulos.h"
#include "los.h"

#define argc _argc
#define argv _argv

int main(int _argc, char **_argv)
{
#ifdef __WIN32__
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
	setlocale(LC_ALL, "");
#endif

	int stream = 0;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0)
			stream = 1;
	}
	emulos_init();

	switch (setjmp(emulos_jb)) {
	case EMULOS_OK:
		break;

	case EMULOS_OUT_OF_MEMORY:
		fputs(emulos_err, stderr);
		fputs("Out of memory\n", stderr);
		return 1;

	case EMULOS_ASSERT:
		fputs(emulos_err, stderr);
		fputs("Assertion fail\n", stderr);
		return 1;
	}

	Los *los = los_create();
	if (los == NULL) {
		fprintf(stderr, "Failed to create los\n");
		return 1;
	}

	if (stream)
		goto stream;

	puts("");
	puts("    ____              __        ___       ,#112104");
	puts("    |                  |   __  /   \\     4$14  0123");
	puts("    ---- |,- ,-  |  |  |  /  \\ \\___      81  __  __\\");
	puts("    |    |  |  | |  |  |  |  |     \\     8 /<o> <o>`|");
	puts("    ---- |  |  | \\__| _|_ \\__/ \\___/     (      \\   /");
	puts("                                          \\    ``  /");
	puts("             Version 0.0.1                 \\   -- /");
	puts("                                           | \\___/");

	char *input;

	while (1) {
		input = readline("> ");
		if (input == NULL)
			goto end;

		puts("");

		int ret = los_tell(los, input);
		free(input);

		if (ret == 0)
			printf("\t%s\n\n", los->reply);
	}

#define INPUT_LIMIT 1024
stream:;
	char buffer[INPUT_LIMIT];
	if (fgets(buffer, INPUT_LIMIT, stdin) == 0)
		goto end;

	los_tell(los, buffer);
	printf("%s\n", los->reply);
	goto stream;
end:
	los_destroy(los);
	return 0;
}
