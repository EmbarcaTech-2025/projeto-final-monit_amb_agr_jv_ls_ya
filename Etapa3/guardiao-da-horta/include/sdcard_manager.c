#include "sdcard_manager.h"

#define BUFFER_SIZE 512   // Tamanho do buffer de leitura do SDcard se tiver memória sobrando pode dobrar isso
#define BUFFER_SIZE2 512*2

void list_dir_content(const char *path){

    FATFS fs;
    FRESULT fr = f_mount(&fs, "", 1);

    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    DIR dir; // "objeto" de diretório
    FILINFO fno; // informações do arquivo
    // FRESULT fr; // resultados das interações com o sistema de arquivos montado

    fr = f_opendir(&dir,path);

    if(fr =! FR_OK){
        printf("Erro ao abrir pasta: %d\n",fr);
    }

    printf("Conteúdo de %s:\n", path);
    printf("-----------------\n");

    // Ler todas as entradas
    while ((fr = f_readdir(&dir, &fno)) == FR_OK && fno.fname[0] != 0) {
        // Verificar se é diretório ou arquivo
        if (fno.fattrib & AM_DIR) {
            printf("[DIR]  %s\n", fno.fname);
        } else {
            printf("[FILE] %s (%lu bytes)\n", fno.fname, fno.fsize);
        }
    }
    
    // Fechar diretório
    f_closedir(&dir);
    
    if (fr != FR_OK) {
        printf("Erro durante leitura: %d\n", fr);
    }

    // Unmount the SD card
    f_unmount("");


}

void sd_write_data(const char *data_str,const char *name_file,const char *path){
    FATFS fs;
    FRESULT fr=f_mount(&fs,"",1);

    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }

     // Open a file and write to it
    FIL fil;
    char filename[30];
    sprintf(filename, "%s.txt",name_file);
    // printf("O arquivo criado terá nome: %s\n",filename);

    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr) {
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    }
    if (f_printf(&fil, "%s\n",data_str) < 0) {
        printf("f_printf failed\n");
    }

    // Close the file
    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    // Unmount the SD card
    f_unmount("");
}

void sd_read_data(char *data_str,const uint str_size,const char *name_file,const char *path){
    FATFS fs;
    FRESULT fr=f_mount(&fs,"",1);

    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    FIL fil;

    fr = f_open(&fil, name_file, FA_READ);
    if (fr != FR_OK) {
        printf("Erro ao abrir arquivo: %d\n", fr);
        return;
    }

    // char buffer[BUFFER_SIZE2];
    UINT bytes_lidos;
    // uint32_t total_bytes_lidos = 0;

    //Faz a leitura do arquivo
    fr = f_read(&fil,data_str,str_size,&bytes_lidos);

    if (fr != FR_OK) {
        printf("Erro na leitura do conteúdo: %d\n",fr);
        return;
    }

    // Close the file
    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    // Unmount the SD card
    f_unmount("");

}