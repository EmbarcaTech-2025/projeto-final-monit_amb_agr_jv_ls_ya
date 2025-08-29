#ifndef SDCARD_MANAGER
#define SDCARD_MANAGER

#include <string.h>
#include<stdio.h>

#include "pico/stdlib.h"

#include "hw_config.h"
#include "f_util.h"
#include "ff.h"

void list_dir_content(const char *path);

//Escreve o conteúdo de 'data_str' no arquvio 'name_file'(sem .txt no fim) no caminho 'path'
void sd_write_data(const char *data_str,const char *name_file,const char *path);

//Lê o conteúdo na string de entrada é flexível com o tamanho sendo determinado nos parâmetros de entrada
void sd_read_data(char *data_str,const uint str_size,const char *name_file,const char *path);

//checa se há SD conectado no módulo
bool sd_card_check();

//pega espaço total e o livre do SDcard em kb
void sd_card_space(bool *mounted,uint64_t *total_kb, uint64_t *free_kb);

#endif