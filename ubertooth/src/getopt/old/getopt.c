/**************************************************************************
*                                                                         *
*     Функция  "getopt"  позиционируется как аналог библиотечной функции  *
*  GCC "getopt".                                                          *
*     Данный    вариант    функции   "getopt"    является    результатом  *
*  реинжиниринга,  то есть, в основе кода "getopt" не лежит код "getopt"  *
*  из библиотеки GCC.                                                     *
*     Данная  функция  "getopt"  практически полностью  копирует  логику  *
*  работы  "getopt" из  библиотеки  GCC, за  исключением двух  известных  *
*  моментов:                                                              *
*                                                                         *
*  1) Сообщения об ошибках, вместо потока "stderr" выводятся в поток      *
*     "stdout".                                                           *
*                                                                         *
*  2) Функция не реагирует на наличие переменной окружения -              *
*     "POSIXLY_CORRECT"                                                   *
*                                                                         *
*                                                                         *
*  int getopt (int argc, char* const* argv, const char* optString)        *
*                                                                         *
*  Вход: argc      - Количество аргументов командной строки (размер       *
*                    массива "argv").                                     *
*                                                                         *
*        argv      - Массив указателей на аргументы командной строки.     *
*                                                                         *
*        optString - Строка со списком коротких опций и символами         *
*                    управляющими работой getopt_long.                    *
*                                                                         *
*  Выход:    Если  найдена  короткая опция,  то функция возвращает ASCII  *
*         код символа этой опции.                                         *
*            Если   короткая  опция  имеет  аргумент,  то  указатель  на  *
*         аргумент помещается в глобальную переменную "optarg".           *
*            При    обнаружении   короткой   опции,   описание   которой  *
*         отсутствует   в  "optString",  функция  возвращает  '?',  а  в  *
*         переменную optopt - помещается ASCII код обнаруженной опции.    *
*            Если  у короткой  опции  отсутствует  аргумент, то  функция  *
*         возвращает либо '?', либо ':', в зависимости от режима работы,  *
*         определяемого   первыми  символами  строки  "optString",  а  в  *
*         переменную optopt - помещается ASCII код обнаруженной опции.    *
*            При обнаружении аргумента командной строки, начинающегося с  *
*         "--"  поиск опций прекращается, функция возвращает -1. Функция  *
*         также  возвращает -1,  если  при поиске  опций достигла  конца  *
*         массива  "argv", а  если  первым  символом строки  "optString"  *
*         является  '+', то  функция возвращает  -1 как  только встретит  *
*         первый аргумент командной строки, не являющийся опцией.         *
*                                                                         *
*                               Примечания                                *
*                                                                         *
*     После  того,  как функция "getopt"   возвратила -1 она находится в  *
*  состоянии  "парсинг завершён", это состояние означает, что при вызове  *
*  "getopt"   -  она  всегда  будет возвращать  -1,  в независимости  от  *
*  значения   глобальной   переменной  "optind".  Для  сброса  состояния  *
*  "парсинг завершён", нужно установить глобальную переменную "optreset"  *
*  в  значение,  отличное  от  0,  тогда  при  вызове  функции  "getopt"  *
*  состояние  "парсинг  завершён"  будет сброшено,  функция  перейдёт  в  *
*  исходное состояние и продолжит свою работу.                            *
*     Если первым символом в строке "optString" является ':', то"getopt"  *
*  не выводит сообщения об ошибках, и в случае если у опции, для которой  *
*  присутствие  аргумента  обязательно, отсутствует  аргумент,  "getopt"  *
*  возвращает ':' вместо '?'.                                             *
*     Если  первый символом в строке "optString" является '+' или '-', а  *
*  символ  ':' является вторым символом, то ':' никак не влияет на вывод  *
*  сообщений   об  ошибках,  а  в  случае  если  у  опции,  для  которой  *
*  присутствие  аргумента  обязательно, отсутствует  аргумент,  "getopt"  *
*  возвращает ':' вместо '?'.                                             *
*     Если первым символом в строке "optString" является '+', то функция  *
*  перейдёт  в состояние  "парсинг  завершён" либо  как только  встретит  *
*  первый  аргумент командной  строки,  не являющийся  опцией, либо  как  *
*  только  встретит аргумент командной строки, начинающийся с "--", либо  *
*  если достигнет конца массива "argv".                                   *
*     Если первым символом в строке "optString" является '-', то функция  *
*  будет  выполнять  поиск опций  либо  до  тех  пор, пока  не  встретит  *
*  аргумент командной строки, начинающийся с "--", либо до тех пор, пока  *
*  не  достигнет конца массива "argv". Однако, в процессе работы функция  *
*  не  будет сортировать массив "argv", перемещая опции и их аргументы в  *
*  начало массива, а не опции в конец массива.                            *
*     Если  первым символом  строки "optString" не является '+' или '-',  *
*  то функция выполняет поиск либо до тех пор, пока не встретит аргумент  *
*  командной  строки, начинающийся  с "--",  либо  до тех  пор, пока  не  *
*  достигнет  конца  массива "argv". При  этом в процессе работы функция  *
*  будет  сортировать массив  "argv" перемещая  опции и  их аргументы  в  *
*  начало массива, а не опции в конец массива.                            *
*     Если   в  глобальную   переменную  "opterr"   записать  ноль,   то  *
*  функция"getopt" не будет выводить сообщения об ошибках.                *
*     Подробнее  об использовании  функции  "getopt"  можно прочесть  по  *
*  адресу: http://www.firststeps.ru/linux/r.php?10                        *
*                                                                         *
*                         Лицензионное соглашение                         *
*                                                                         *
*     Данная программа распространяется как есть. Вы можете использовать  *
*  программу  на свой  риск. Автор программы не несёт ответственности за  *
*  любой   ущерб  прямо  или  косвенно  причинённый  данной  программой.  *
*  Используя данную программу, Вы соглашаетесь с данным соглашением.      *
*                                                                         *
*                                         Copyright (c) Евгений Соловьёв  *
*                                               eugen-soloviov@yandex.ru  *
*                                                                         *
***************************************************************************/

#include "getopt.h"
#include <stdio.h>

#define bool int
#define true 1
#define false 0

extern const char getopt_argument_error[];
extern const char getopt_option_error[];
extern const char getopt_printf_option[];

extern char* _cdecl _find_app_name (char*);

int getopt (int argc, char* const* argv, const char* optString)
 {
   static bool Parsing_Done = false; // Парсинг завершён
   
   static unsigned int first_nonopt = 0;     
   static unsigned int last_opt = 0;
   static unsigned int j = 1;
   
   static char* app_name = 0;    // Указатель на имя приложения

/*--------------------- Флаги режима работы getopt ----------------------*/
   
   bool Stop_At_Fist_Non_Option;   /* Останавливать парсинг при появлении 
                                      первого аргумента неопции */
   
   bool Permute_Premission;        // Разрешение престановок в argv
   
   bool Mark_Argument_Error;       /* Делать различие между неверной опцией 
                                      и отсутствием аргумента */
   
   bool Error_Messages_Premission; // Разрешение вывода сообщений об ошибках
                                       
/*-----------------------------------------------------------------------*/   
   
   int current_value, next_value;
   unsigned int i;

   if (optreset)
    {
      // Сброс getopt в исходное состояние
      
      first_nonopt = 0;
      last_opt = 0;
      optreset = 0;
      j = 1;
      Parsing_Done = false;
    }
   
   if (Parsing_Done)
    return -1;
   
   
   /* Анализ начальных байтов строки "optString" и установка флагов режима работы
      getopt */
   
   if (optString[0] == '-') 
    {
      Stop_At_Fist_Non_Option = false;
      Permute_Premission = false;
      optString++;
    }
   else
    if (optString[0] == '+') 
     {
       Stop_At_Fist_Non_Option = true;
       Permute_Premission = false;
       optString++;
     }
    else
     {
       Stop_At_Fist_Non_Option = false;
       Permute_Premission = true;
     }

   if (optString[0] == ':')
    {
      Mark_Argument_Error = true;
      optString++;
    }
   else
    Mark_Argument_Error = false;
  
   Error_Messages_Premission = opterr && ( !Mark_Argument_Error
                                           ||
                                           !Permute_Premission );
   
   optarg = 0;
   
   if (!app_name)
    app_name = _find_app_name(argv[0]); // Инициализация переменной app_name
   

   if (Permute_Premission && first_nonopt && (j == 1))
    _asm call _argv_permute;  // Выполнение перестановок в argv
      
  
   if (j > 1) 
    goto _short_option_parsing;

   
   // Цикл поиска опций в аргументах командной строки
      
   for (;; optind++)
    {
      if ( (unsigned int)optind >= (unsigned int)argc )
       {
         // Переход в состояние "парсинг завершён"

_parsing_complete:      

         if (Permute_Premission && first_nonopt)
           optind = first_nonopt;
 
         Parsing_Done = true;
         return -1;
       }

      
      current_value = *argv[optind];
      next_value = *(argv[optind]+1);
      
      if (current_value == '-' && next_value != '\0')
       break;
      
      if (Stop_At_Fist_Non_Option)
       goto _parsing_complete;
      
      if (!first_nonopt)
       first_nonopt = optind;
     
    }

      
   last_opt = optind;
   
   if (next_value != '-')
    {
      // Обработка коротких опций  
      
_short_option_parsing:       
      
      current_value = *(argv[optind]+j);
      
      
      // Цикл поиска короткой опции
      
      for (i = 0 ; ; i++)
       {
         char cc;
         cc = optString[i];
         
         if (cc == '\0')
          {
            // Ошибка - неопределённая короткая опция
           
            if (Error_Messages_Premission)
             printf (getopt_printf_option, app_name, getopt_option_error, &current_value);
            
            optopt = current_value;
            j++;
            if (*(argv[optind]+j) == '\0')
             {
               j = 1;
               optind++;
             }
            
            return '?';
          }
         
         if (cc == ':' || cc == ';')
          continue;
         
         if (current_value == cc)
          break;
            
       }
      
       
       if (optString[i+1] == ':')
        {
          if (optString[i+2] != ':')
           {
             // Короткая опция имеет обязательный аргумент
           
             j++;
             if (*(argv[optind]+j) != 0)
              optarg = argv[optind]+j;
             else
              if ( (unsigned int)optind <= (unsigned int)(argc - 2) )
               {
                 optarg = argv[optind+1];
                 optind++;
               }
              else
               {
                 // Ошибка - отсутствует аргумент короткой опции
               
                 if (Error_Messages_Premission)
                  printf (getopt_printf_option, app_name, getopt_argument_error, &current_value);
            
                 optopt = current_value;
                 
                 if (Mark_Argument_Error) 
                  current_value = ':';
                 else
                  current_value = '?';
               }
             
             goto _short_option_parsing_end;
           }
          else
           {
             // Короткая опция имеет необязательный аргумент
             
             j++;
             if (*(argv[optind]+j) != 0)
              optarg = argv[optind]+j;
              
             goto _short_option_parsing_end;
           }
        }
      
      
      // Короткая опция не имеет аргументов
      
      if (current_value != 'W' && optString[i+1] != ';') /* Если короткая опция 'W' и в
                                                           "optString" определено "W;", 
                                                           то интерпретируем "-W.." , 
                                                           как длинную опцию    */
       {
         // Заканчиваем обработку коротких опций без аргументов
         
         j++;
      
         if (*(argv[optind]+j) != '\0')
          return current_value;
         else 
          {
_short_option_parsing_end:             

            optind++;
            j = 1;
           
            return current_value;
          }
       }
    }

   
   /* Завершение парсинга в случае если argv[optind] является длинной опцией 
      или ASCIIZ-строкой  "--" */
      
   optind++;
   
   if (Permute_Premission && first_nonopt)
    _asm call _argv_permute; // Выполнение перестановок в argv

   goto _parsing_complete;


/**************************************************************************
*                                                                         *
*     Подпрограмма  "_argv_permute"  - выполняет перестановку аргументов  *
*  командной  строки,  постепенно группируя  все обработанные опции и их  *
*  аргументы  в  начале массива "argv", а все аргументы не опции в конце  *
*  массива "argv".                                                        *
*                                                                         *
**************************************************************************/

_argv_permute:
   {
     char* aa;
     char* cc;
      
     while (last_opt < (unsigned int) optind)
      {
        aa = argv[last_opt];
        for (i = last_opt; i > first_nonopt; i--)
         {
           cc = argv[i-1];
           *(char**)&argv[i] = cc;
         }
        *(char**)&argv[first_nonopt] = aa;
        last_opt++;
        first_nonopt++;
      }
   }
  _asm  ret;
  
 }
