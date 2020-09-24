/*!  \file t-str-macosx.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_T_STR_MACOSX_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_T_STR_MACOSX_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

char * itoa (int val, char *buf, int radix);
char * ltoa (long val, char *buf, int radix);
char * ultoa (unsigned long val, char *buf, int radix);

#define t_strcpy   strcpy
#define t_strncpy  strncpy
#define t_strcat   strcat
#define t_strcmp   strcmp
#define t_stricmp  strcasecmp
#define t_strncmp  strncmp
#define t_strnicmp strncasecmp
#define t_strlen   strlen
#define t_strchr   strchr
#define t_strrchr  strrchr
#define t_strstr   strstr
#define t_strtok   strtok

#define t_printf   printf
#define t_sprintf  sprintf
#define t_fprintf  fprintf
#define t_vsprintf vsprintf
#define t_vprintf  vprintf
#define t_vfprintf vfprintf

#define t_scanf    scanf
#define t_sscanf   sscanf
#define t_fscanf   fscanf

#define t_toupper  toupper
#define t_tolower  tolower

#define t_atoi     atoi
#define t_atol     atol
#define t_atof     atof
#define t_itoa     itoa

#define t_isalnum  isalnum
#define t_isalpha  isalpha
#define t_isdigit  isdigit
#define t_isspace  isspace

#define t_fopen    fopen
#define t_remove   remove
#define t_rename   rename
#define t_getcwd   getcwd

#define t_fgets    fgets
#define t_fputs    fputs
#define t_fputc    fputc

#define t_ftruncate ftruncate
#define t_fileno    fileno

#define t_getenv   getenv
#define t_mkdir    mkdir

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_T_STR_MACOSX_H

