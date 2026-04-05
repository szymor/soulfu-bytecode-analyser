// SoulFu Byte Code Analyser

#include <stdio.h>
#include <stdlib.h>

#define RUN_BUFFER_SIZE              65550
#define MAX_FAST_FUNCTION            16
#define MAX_VARIABLE                 32   // I00 - I31, F00 - F31...

struct LabelDesc {
    unsigned short address;
    const char *label;
};

unsigned char run_buffer[RUN_BUFFER_SIZE];
unsigned int run_buffer_used;

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

const char opcode_names[][32] = {
    "EQUALS",
    "ADD",
    "SUBTRACT",
    "MULTIPLY",
    "DIVIDE",
    "INCREMENT",
    "DECREMENT",
    "ISEQUAL",
    "ISNOTEQUAL",
    "ISGREATEREQUAL",
    "ISLESSEREQUAL",
    "ISGREATER",
    "ISLESSER",
    "LOGICALAND",
    "LOGICALOR",
    "LOGICALNOT",
    "NEGATE",
    "ERRx11",
    "ERRx12",
    "BITWISEAND",
    "BITWISEOR",
    "BITWISELEFT",
    "BITWISERIGHT",
    "MODULUS",
    "LOCALMESSAGE",
    "LOGMESSAGE",
    "FINDSELF",
    "SYSTEMSET",
    "SYSTEMGET",
    "DEBUGMESSAGE",
    "TOFLOAT",
    "TOINT",
    "F_EQUALS",
    "F_ADD",
    "F_SUBTRACT",
    "F_MULTIPLY",
    "F_DIVIDE",
    "F_INCREMENT",
    "F_DECREMENT",
    "F_ISEQUAL",
    "F_ISNOTEQUAL",
    "F_ISGREATEREQUAL",
    "F_ISLESSEREQUAL",
    "F_ISGREATER",
    "F_ISLESSER",
    "F_LOGICALAND",
    "F_LOGICALOR",
    "F_LOGICALNOT",
    "F_NEGATE",
    "STRING",
    "STRINGGETNUMBER",
    "STRINGCLEAR",
    "STRINGCLEARALL",
    "STRINGAPPEND",
    "STRINGCOMPARE",
    "STRINGLENGTH",
    "STRINGCHOPLEFT",
    "STRINGCHOPRIGHT",
    "STRINGRANDOMNAME",
    "STRINGSANITIZE",
    "NETWORKMESSAGE",
    "STRINGLANGUAGE",
    "STRINGUPPERCASE",
    "STRINGAPPENDNUMBER",
    "CALLFUNCTION",
    "RETURNINT",
    "RETURNFLOAT",
    "IFFALSEJUMP",
    "JUMP",
    "SQRT",
    "FILEOPEN",
    "FILEREADBYTE",
    "FILEWRITEBYTE",
    "FILEINSERT",
    "SPAWN",
    "GOPOOF",
    "DISMOUNT",
    "ROLLDICE",
    "PLAYSOUND",
    "PLAYMEGASOUND",
    "DISTANCESOUND",
    "PLAYMUSIC",
    "UPDATEFILES",
    "SIN",
    "ACQUIRETARGET",
    "FINDPATH",
    "BUTTONPRESS",
    "AUTOAIM",
    "ROOMHEIGHTXY",
    "ERRx59",
    "ERRx5A",
    "ERRx5B",
    "ERRx5C",
    "ERRx5D",
    "ERRx5E",
    "ERRx5F",
    "WINDOWBORDER",
    "WINDOWSTRING",
    "WINDOWMINILIST",
    "WINDOWSLIDER",
    "WINDOWIMAGE",
    "WINDOWTRACKER",
    "WINDOWBOOK",
    "WINDOWINPUT",
    "WINDOWEMACS",
    "WINDOWMEGAIMAGE",
    "WINDOW3DSTART",
    "WINDOW3DEND",
    "WINDOW3DPOSITION",
    "WINDOW3DMODEL",
    "MODELASSIGN",
    "PARTICLEBOUNCE",
    "WINDOWEDITKANJI",
    "WINDOW3DROOM",
    "INDEXISLOCALPLAYER",
    "FINDBINDING",
    "FINDTARGET",
    "FINDOWNER",
    "FINDINDEX",
    "FINDBYINDEX",
    "FINDWINDOW",
    "FINDPARTICLE",
    "ERRx7A",
    "ATTACHTOTARGET",
    "GETDIRECTION",
    "DAMAGETARGET",
    "EXPERIENCEFUNCTION",
    "ERRx7F"
};

enum Opcode {
    OPCODE_EQUALS,
    OPCODE_ADD,
    OPCODE_SUBTRACT,
    OPCODE_MULTIPLY,
    OPCODE_DIVIDE,
    OPCODE_INCREMENT,
    OPCODE_DECREMENT,
    OPCODE_ISEQUAL,
    OPCODE_ISNOTEQUAL,
    OPCODE_ISGREATEREQUAL,
    OPCODE_ISLESSEREQUAL,
    OPCODE_ISGREATER,
    OPCODE_ISLESSER,
    OPCODE_LOGICALAND,
    OPCODE_LOGICALOR,
    OPCODE_LOGICALNOT,
    OPCODE_NEGATE,
    OPCODE_ERRx11,
    OPCODE_ERRx12,
    OPCODE_BITWISEAND,
    OPCODE_BITWISEOR,
    OPCODE_BITWISELEFT,
    OPCODE_BITWISERIGHT,
    OPCODE_MODULUS,
    OPCODE_LOCALMESSAGE,
    OPCODE_LOGMESSAGE,
    OPCODE_FINDSELF,
    OPCODE_SYSTEMSET,
    OPCODE_SYSTEMGET,
    OPCODE_DEBUGMESSAGE,
    OPCODE_TOFLOAT,
    OPCODE_TOINT,
    OPCODE_F_EQUALS,
    OPCODE_F_ADD,
    OPCODE_F_SUBTRACT,
    OPCODE_F_MULTIPLY,
    OPCODE_F_DIVIDE,
    OPCODE_F_INCREMENT,
    OPCODE_F_DECREMENT,
    OPCODE_F_ISEQUAL,
    OPCODE_F_ISNOTEQUAL,
    OPCODE_F_ISGREATEREQUAL,
    OPCODE_F_ISLESSEREQUAL,
    OPCODE_F_ISGREATER,
    OPCODE_F_ISLESSER,
    OPCODE_F_LOGICALAND,
    OPCODE_F_LOGICALOR,
    OPCODE_F_LOGICALNOT,
    OPCODE_F_NEGATE,
    OPCODE_STRING,
    OPCODE_STRINGGETNUMBER,
    OPCODE_STRINGCLEAR,
    OPCODE_STRINGCLEARALL,
    OPCODE_STRINGAPPEND,
    OPCODE_STRINGCOMPARE,
    OPCODE_STRINGLENGTH,
    OPCODE_STRINGCHOPLEFT,
    OPCODE_STRINGCHOPRIGHT,
    OPCODE_STRINGRANDOMNAME,
    OPCODE_STRINGSANITIZE,
    OPCODE_NETWORKMESSAGE,
    OPCODE_STRINGLANGUAGE,
    OPCODE_STRINGUPPERCASE,
    OPCODE_STRINGAPPENDNUMBER,
    OPCODE_CALLFUNCTION,
    OPCODE_RETURNINT,
    OPCODE_RETURNFLOAT,
    OPCODE_IFFALSEJUMP,
    OPCODE_JUMP,
    OPCODE_SQRT,
    OPCODE_FILEOPEN,
    OPCODE_FILEREADBYTE,
    OPCODE_FILEWRITEBYTE,
    OPCODE_FILEINSERT,
    OPCODE_SPAWN,
    OPCODE_GOPOOF,
    OPCODE_DISMOUNT,
    OPCODE_ROLLDICE,
    OPCODE_PLAYSOUND,
    OPCODE_PLAYMEGASOUND,
    OPCODE_DISTANCESOUND,
    OPCODE_PLAYMUSIC,
    OPCODE_UPDATEFILES,
    OPCODE_SIN,
    OPCODE_ACQUIRETARGET,
    OPCODE_FINDPATH,
    OPCODE_BUTTONPRESS,
    OPCODE_AUTOAIM,
    OPCODE_ROOMHEIGHTXY,
    OPCODE_ERRx59,
    OPCODE_ERRx5A,
    OPCODE_ERRx5B,
    OPCODE_ERRx5C,
    OPCODE_ERRx5D,
    OPCODE_ERRx5E,
    OPCODE_ERRx5F,
    OPCODE_WINDOWBORDER,
    OPCODE_WINDOWSTRING,
    OPCODE_WINDOWMINILIST,
    OPCODE_WINDOWSLIDER,
    OPCODE_WINDOWIMAGE,
    OPCODE_WINDOWTRACKER,
    OPCODE_WINDOWBOOK,
    OPCODE_WINDOWINPUT,
    OPCODE_WINDOWEMACS,
    OPCODE_WINDOWMEGAIMAGE,
    OPCODE_WINDOW3DSTART,
    OPCODE_WINDOW3DEND,
    OPCODE_WINDOW3DPOSITION,
    OPCODE_WINDOW3DMODEL,
    OPCODE_MODELASSIGN,
    OPCODE_PARTICLEBOUNCE,
    OPCODE_WINDOWEDITKANJI,
    OPCODE_WINDOW3DROOM,
    OPCODE_INDEXISLOCALPLAYER,
    OPCODE_FINDBINDING,
    OPCODE_FINDTARGET,
    OPCODE_FINDOWNER,
    OPCODE_FINDINDEX,
    OPCODE_FINDBYINDEX,
    OPCODE_FINDWINDOW,
    OPCODE_FINDPARTICLE,
    OPCODE_ERRx7A,
    OPCODE_ATTACHTOTARGET,
    OPCODE_GETDIRECTION,
    OPCODE_DAMAGETARGET,
    OPCODE_EXPERIENCEFUNCTION,
    OPCODE_ERRx7F
};

unsigned short read_short_be(unsigned char *ptr)
{
    return (ptr[0] << 8) | ptr[1];
}

unsigned int read_int_be(unsigned char *ptr)
{
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

void go_past_null(unsigned char **ptrptr)
{
    while (*(*ptrptr)++);
}

const char *get_type_related_string(unsigned char letter)
{
    static const char ci[] = "int";
    static const char cf[] = "float";
    static const char cw[] = "WRONG_TYPE";

    switch (letter)
    {
        case 'I': return ci;
        case 'F': return cf;
        default: return cw;
    }
}

int print_arg_data(char **opsptr, char **bptr, unsigned char *argptr)
{
    int bytes_consumed = 0;

    switch (*argptr & 224)
    {
        case 128:   // load int variable
        {
            int var_index = *argptr & (MAX_VARIABLE - 1);
            *bptr   += sprintf(*bptr, "%02x", *argptr);
            *opsptr += sprintf(*opsptr, " I%02d", var_index);
            bytes_consumed = 1;
            break;
        }
        case 160:   // load property
        {
            int var_index = argptr[0] & (MAX_VARIABLE - 1);
            int property_number = argptr[1];
            *bptr   += sprintf(*bptr, "%02x%02x", argptr[0], argptr[1]);
            *opsptr += sprintf(*opsptr, " [I%02d].%d", var_index, property_number);
            // TODO: write property offsets and types based on property_offset and property_type dynamic data
            bytes_consumed = 2;
            break;
        }
        case 192:   // load float variable
        {
            int var_index = *argptr & (MAX_VARIABLE - 1);
            *bptr   += sprintf(*bptr, "%02x", *argptr);
            *opsptr += sprintf(*opsptr, " F%02d", var_index);
            bytes_consumed = 1;
            break;
        }
        default:
        {
            *bptr   += sprintf(*bptr, "%02x", *argptr);
            *opsptr += sprintf(*opsptr, " ERR");
            bytes_consumed = 1;
        }
    }

    return bytes_consumed;
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

    // load the RUN file
    FILE *runfile = fopen(argv[1], "rb");
    if (!runfile)
    {
        fprintf(stderr, "File not found.\n");
        return EXIT_FAILURE;
    }
    run_buffer_used = fread(run_buffer, 1, RUN_BUFFER_SIZE, runfile);
    fclose(runfile);

    printf("File %s, loaded %u bytes.\n", argv[1], run_buffer_used);
    unsigned char *runptr = run_buffer;

    // print fast functions
    int no_ff = 1;
    for (int i = 0; i < MAX_FAST_FUNCTION; ++i)
    {
        unsigned short offset = read_short_be(runptr);
        runptr += 2;
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

    int function_count = read_short_be(runptr);
    runptr += 2;
    int string_count = read_short_be(runptr);
    runptr += 2;

    struct LabelDesc *label_descs =
      (struct LabelDesc *)malloc(function_count * sizeof(struct LabelDesc));

    // list functions
    if (function_count > 0)
    {
        printf("%d function(s):\n", function_count);
        for (int i = 0; i < function_count; ++i)
        {
            const char *type_string = NULL;

            unsigned short address = read_short_be(runptr);
            runptr += 2;
            label_descs[i].address = address;

            const char *function_name = runptr;
            go_past_null(&runptr);
            label_descs[i].label = function_name;

            const char *args = runptr;
            go_past_null(&runptr);

            type_string = get_type_related_string(*args++);

            printf("  %04x %s %s(", address, type_string, function_name);
            if (*args)
            {
                while (*args)
                {
                    type_string = get_type_related_string(*args++);
                    printf("%s", type_string);
                    if (*args)
                    {
                        printf(", ");
                    }
                }
            }
            printf(")\n");
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
            // dummy bytes
            runptr += 2;

            printf("  %s\n", runptr);
            go_past_null(&runptr);
        }
    }
    else
    {
        printf("No strings defined.\n");
    }

    // interpret byte code
    char opstring[256];
    char bstring[256];
    while (runptr < (run_buffer + run_buffer_used))
    {
        unsigned short listing_offset = runptr - run_buffer;
        // check whether the offset is in function offset list
        for (int i = 0; i < function_count; ++i)
        {
            if (listing_offset == label_descs[i].address)
            {
                printf("\n%s:\n", label_descs[i].label);
            }
        }
        printf("%04x:", runptr - run_buffer);

        char *opsptr = opstring;
        char *bptr = bstring;

        if (*runptr < 128)
        {
            bptr   += sprintf(bptr, "%02x", *runptr);
            opsptr += sprintf(opsptr, "%s", opcode_names[*runptr]);
            switch (*runptr)
            {
                case OPCODE_EQUALS:        // puts top value from int stack into the given variable (1 byte extension)
                case OPCODE_INCREMENT:
                case OPCODE_DECREMENT:
                case OPCODE_F_EQUALS:
                case OPCODE_F_INCREMENT:
                case OPCODE_F_DECREMENT:
                    ++runptr;
                    runptr += print_arg_data(&opsptr, &bptr, runptr);
                    break;
                case OPCODE_CALLFUNCTION:
                    ++runptr;
                    /* Arguments:
                     * 4 byte jump location
                     *   to be verified if it always corresponds to the function name
                     * 4 byte return position
                     *   notice that potentially it could be used to jump to the instruction other than the next one!
                     *   to be verified how it is updated in runtime
                     *   (probably just moving the pointer after the function name)
                     * 4 byte argument list pointer
                     * 4 byte file start address
                     * zero-terminated function name
                    */
                    bptr   += sprintf(bptr, "..");
                    opsptr += sprintf(opsptr, " %08x %08x %08x %08x \"%s\"", read_int_be(runptr), read_int_be(runptr + 4), read_int_be(runptr + 8), read_int_be(runptr + 12), runptr + 16);
                    runptr += 16;
                    go_past_null(&runptr);
                    break;
                case OPCODE_IFFALSEJUMP:     // goes to a new read address if the top integer isn't TRUE
                case OPCODE_JUMP:
                    ++runptr;
                    bptr   += sprintf(bptr, "%02x%02x", runptr[0], runptr[1]);
                    opsptr += sprintf(opsptr, " %04x", read_short_be(runptr));     // address relative to the beginning of the file
                    // TODO: resolve addresses to dynamic jump labels and function labels
                    runptr += 2;
                    break;
                default:
                    ++runptr;
            }
        }
        else
        {
            // operands
            switch (*runptr & 224)
            {
                case 128: // integer
                case 160: // property
                case 192: // float
                    bptr   += sprintf(bptr, "%02x", *runptr);
                    opsptr += sprintf(opsptr, "PUSH");
                    runptr += print_arg_data(&opsptr, &bptr, runptr);
                    break;
                case 224: // extended opcode
                    bptr   += sprintf(bptr, "%02x", *runptr);
                    opsptr += sprintf(opsptr, "PUSH");
                    switch (*runptr & 31)
                    {
                        case 0:
                            opsptr += sprintf(opsptr, " 0");
                            break;
                        case 1:
                            opsptr += sprintf(opsptr, " 1");
                            break;
                        case 2:
                            opsptr += sprintf(opsptr, " 1.0f");
                            break;
                        case 3:
                            bptr   += sprintf(bptr, "%02x", runptr[1]);
                            opsptr += sprintf(opsptr, " BYTE %02x", runptr[1]);
                            ++runptr;
                            break;
                        case 4:
                            bptr   += sprintf(bptr, "%02x%02x", runptr[1], runptr[2]);
                            opsptr += sprintf(opsptr, " SHORT %04x", read_short_be(runptr + 1));
                            runptr += 2;
                            break;
                        case 5:
                            bptr   += sprintf(bptr, "%02x%02x%02x%02x", runptr[1], runptr[2], runptr[3], runptr[4]);
                            opsptr += sprintf(opsptr, " INT %08x", read_int_be(runptr + 1));
                            runptr += 4;
                            break;
                        case 6:
                            unsigned int fnum = read_int_be(runptr + 1);
                            bptr   += sprintf(bptr, "%02x%02x%02x%02x", runptr[1], runptr[2], runptr[3], runptr[4]);
                            opsptr += sprintf(opsptr, " FLOAT %f", *((float *)&fnum));
                            runptr += 4;
                            break;
                        case 7:
                            opsptr += sprintf(opsptr, " STRING (?)");
                            break;
                        case 8:
                            bptr   += sprintf(bptr, "%02x%02x%02x%02x", runptr[1], runptr[2], runptr[3], runptr[4]);
                            opsptr += sprintf(opsptr, " FILE PTR %08x", read_int_be(runptr + 1));
                            runptr += 4;
                            break;
                        case 9:
                            bptr   += sprintf(bptr, "%02x%02x%02x%02x", runptr[1], runptr[2], runptr[3], runptr[4]);
                            opsptr += sprintf(opsptr, " FILE DATA REF %08x", read_int_be(runptr + 1));
                            runptr += 4;
                            break;
                        default:
                            opsptr += sprintf(opsptr, " INVALID");
                    }
                    ++runptr;
                    break;
                default:  // invalid
                    opsptr += sprintf(opsptr, "INVALID OPCODE");
            }
        }
        printf("  %-10s  %s\n", bstring, opstring);
    }

    free(label_descs);
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
 * 229 opcodes get changed to 233 opcodes (file data ref), if they point to "FILE:" type of strings (to be confirmed)
 */
