// SoulFu Byte Code Analyser

#include <stdio.h>
#include <stdlib.h>

#define MAX_FAST_FUNCTION            16

const char fast_function_names[][32] = {
	"Spawn()",
	"Refresh()",
	"Event()",
	"AIScript()",
	"ButtonEvent()",
	"GetName()",
	"Unpressed()",
	"FrameEvent()",
	"ModelSetup()",
	"DefenseRating()",
	"Setup()",
	"DirectUsage()",
	"EnchantUsage()",
	"FFx0D()",
	"FFx0E()",
	"FFx0F()"
};

unsigned char read_byte(FILE *f)
{
	unsigned char result;

	fread(&result, 1, 1, f);

	return result;
}

unsigned short read_short_be(FILE *f)
{
	unsigned short result = 0;
	unsigned char tmp;

	fread(&tmp, 1, 1, f);
	result = tmp;
	fread(&tmp, 1, 1, f);
	result <<= 8;
	result |= tmp;

	return result;
}

unsigned int read_int_be(FILE *f)
{
	unsigned int result = 0;
	unsigned char tmp;

	fread(&tmp, 1, 1, f);
	result = tmp;
	fread(&tmp, 1, 1, f);
	result <<= 8;
	result |= tmp;
	fread(&tmp, 1, 1, f);
	result <<= 8;
	result |= tmp;
	fread(&tmp, 1, 1, f);
	result <<= 8;
	result |= tmp;

	return result;
}

void read_string(FILE *f, char *buf)
{
	do {
		fread(buf, 1, 1, f);
	} while (*buf++);
}

int main(int argc, char *argv[])
{
	printf("SoulFu Byte Code Analyser\n\n");

	if (argc < 2)
	{
		fprintf(stderr, "Too few arguments.\n");
		printf("Usage: %s [RUN file]\n", argv[0]);
		return EXIT_FAILURE;
	}

	// parse and print RUN-related data
	FILE *runfile = fopen(argv[1], "rb");
	if (!runfile)
	{
		fprintf(stderr, "File not found.\n");
		return EXIT_FAILURE;
	}

	printf("File: %s\n", argv[1]);

	// print fast functions
	int no_ff = 1;
	for (int i = 0; i < MAX_FAST_FUNCTION; ++i)
	{
		unsigned short offset = read_short_be(runfile);
		if (offset)
		{
			if (no_ff)
			{
				printf("Fast functions:\n");
				no_ff = 0;
			}
			printf("  %04x %s\n", offset, fast_function_names[i]);
		}
	}
	if (no_ff)
	{
		printf("No fast functions filled in. The file not functionized.\n");
	}

	int function_count = read_short_be(runfile);
	int string_count = read_short_be(runfile);

	// list functions
	if (function_count > 0)
	{
		printf("%d function(s):\n", function_count);
		for (int i = 0; i < function_count; ++i)
		{
			const char ci[] = "int";
			const char cf[] = "float";
			const char cw[] = "WRONG_TYPE";
			const char *type_string = cw;

			unsigned short address = read_short_be(runfile);

			char function_name[64];
			read_string(runfile, function_name);

			char args[64];
			read_string(runfile, args);

			char *c = args;
			switch (*c++)
			{
				case 'I': type_string = ci; break;
				case 'F': type_string = cf; break;
				default: type_string = cw;
			}

			printf("  %04x %s %s(", address, type_string, function_name);
			if (*c)
			{
				while (*c)
				{
					switch (*c++)
					{
						case 'I': type_string = ci; break;
						case 'F': type_string = cf; break;
						default: type_string = cw;
					}

					if (*c)
					{
						printf("%s, ", type_string);
					}
					else
					{
						printf("%s)\n", type_string);
					}
				}
			}
			else
			{
				printf(")\n");
			}
		}
	}
	else
	{
		printf("No functions imported.\n");
	}

	// list strings
	if (string_count > 0)
	{
		printf("%d string(s):\n", string_count);
		for (int i = 0; i < string_count; ++i)
		{
			char buff[512];

			// dummy bytes
			read_short_be(runfile);

			read_string(runfile, buff);
			printf("  %s\n", buff);
		}
	}
	else
	{
		printf("No strings defined.\n");
	}

	fclose(runfile);
	return EXIT_SUCCESS;
}

/* RUN FILE FORMAT:
 * 
 * Numbers are stored as big endian.
 * 
 * HEADER (output of src_headerize):
 * Fast Function offsets: 16 Fast Functions, 2 byte per Fast Function, 32 bytes in total
 * Function count: 2 bytes
 * String count: 2 bytes
 * List of function names in format: <address> (2 bytes), <fname>, \0, <retcode> ('I' for int, 'F' for float), <list of argument types> (IIFFI...), \0
 * List of strings in format: \xff, \xff, <string>, \0
 * 
 * Byte code: take a look at src_generate_opcodes for details.
 * Opcodes containing additional data after the opcode itself:
 * OPCODE_CALLFUNCTION: 4 byte jump location, 4 byte return position, 4 byte argument list pointer, 4 byte file start address, zero-terminated function name
 * OPCODE_IFFALSEJUMP: 2 byte address
 * token_opcode[token]&0xE0) == 0xA0 (101.....), integer variable with dot parameter, one byte index follows
 * token_opcode[token] == 227, integer numeric token, 1 byte data follows (signed)
 * token_opcode[token] == 228, integer numeric token, 2 byte data follows (signed)
 * token_opcode[token] == 229, integer numeric token, 4 byte data follows (signed)
 * token_opcode[token] == 230, float numeric token, 4 byte data follows (signed)
 * 
 * 229 opcodes get changed to 233 opcodes (file data ref), if they point to "FILE:" type of strings
 */
