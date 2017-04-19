#define bool int
#define true 1
#define false 0

int	opterr = 1;   // Разрешение вывода сообщений об ошибках
int	optind = 1;   // Индекс аргумента командной строки подлежащего обработке
int	optopt = '?';
int	optreset = 0; // Сброс getopt
char    *optarg = 0;  // Указатель на аргумент опции

const char getopt_argument_error[] = ": option requires an argument --";
const char getopt_option_error[] = ": unknown option --";
const char getopt_printf_option[] = "%s%s %s\n";


/**************************************************************************
*                                                                         *
*     Функция  _find_app_name -  ищет имя файла в строке вида: "путь\имя  *
*  файла" и возвращает указатель на имя файла.                            *
*                                                                         *
**************************************************************************/

char* _find_app_name (char* argv_0)
 {
   char * pointer = argv_0;
   char * i;

   for (i = argv_0; *i != 0; i++)
    {
      if (*i == '\\')
       pointer = i;
    }

   return pointer+1;
 }
