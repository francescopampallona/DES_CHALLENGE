#include <iostream>
#include <openssl/des.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>

/**
 * Compilare con "g++ -Wall brute_force.cpp -lssl -lcrypto -o  bin/brute_force"
*/


/*  COMBINAZIONI CON RIPETIZIONE IN 8 POSTI CON 28 ELEMENTI
    RITORNA LA COMBINAZIONE ALLA POSIZIONE N       */
char* combo(char* charset, long long  N){
    char* combo = (char*) malloc(8*sizeof(char));
    //Init
    for(int i=0;i<8;i++){
        combo[i] = charset[0];
    }
    
    //Controllo valore
    for(int c=7; c>=0; c--){
        if(N>=(long long)pow(28, c)){
            int pos = N/(long long )pow(28, c);
            combo[7-c]= charset[pos];
            N = N - pos * (long long)pow(28,c);
            
        }

            
    }
    return combo;
}

/* DECRYPT DES */
char* Decrypt(char* key,char*  cipher_text){
    char* plain_text = (char*)malloc(8*sizeof(char));
    DES_cblock key2;
    DES_key_schedule schedule;
    //Prepare the key
    memcpy(key2, key, 8);
    DES_set_odd_parity(&key2);
    DES_set_key_checked(&key2, &schedule);
    // Decrypt ECB
    DES_ecb_encrypt((unsigned char (*)[8]) cipher_text, (unsigned char (*)[8]) plain_text, &schedule, DES_DECRYPT);
    return (plain_text);

}
/** CHECK CHARACTERS
 * return false if the string contain special characters
*/
bool check(char* plain_text){
    for(int i = 0;i<8;i++){
        if(plain_text[i]<65 || (plain_text[i]>90 && plain_text[i]<97) || plain_text[i]>122){
            return false;

        }

    }
    return true;
}
/** LOAD LIST OF WORDS*/
void load(std::list<char*> *listaParole){
     
    FILE *f = fopen("otto_lettere.txt", "r");
    if (f == NULL) {
        perror("fopen()");
        
    }
   
   
    for(int cnt = 0; feof(f)==0; cnt++){
        
        char * parola = (char*)malloc(8*sizeof(char));
        fscanf(f, " %[^\n]",parola);
        //printf("%s",parola[cnt]);
        (*listaParole).push_back(parola);
        
    }
    
    
    //Chiudo il file*/
    fclose(f);

}
/**RICERCA PAROLA*/
bool trova(std::list<char*> listaParole,char* plain_text){
    bool trovata= false;
    for(char* parola: listaParole){
        for(int i=0;i<8;i++){
            if(plain_text[i]==parola[i] || plain_text[i]==parola[i]+32){
                trovata = true;
                
            }
            else{
                trovata = false;
                break;
            }
        }
        if(trovata==true){
            break;
        }
        
    }
     return trovata;
}
   


    

int main(){
    char charset[29] = {'a','b','d','f','h','j','l','n','p','r','t','v','x','z','A','B','D','F','H','J','L','N','P','R','T','V','X','Z', '\0'};
    char * cipher_text;
    printf("%s\n", charset);
    cipher_text = (char*)malloc(8*sizeof(char));
    //memcpy(cipher_text, "tJR\x14OC\xe2\xeb", 8); //Cipher text di prova
    memcpy(cipher_text, "v\x98\xb8s\xe3Hg\x13", 8); // Cipher_text della challenge

    std::list <char* > listaParole;
    load(&listaParole);
    //printf("%s \n", cipher_text);
    // TENTATIVI
    for(long long tentativo = 2500000000; tentativo<=300000000000;tentativo++){
        char* chiave_provata = combo(charset, tentativo);
        char * plain_text = Decrypt(chiave_provata, cipher_text);
        if(tentativo%100000000==0){ 
            printf("Tentativo: %lld  Chiave provata: %s Plain text: %s\n",tentativo ,chiave_provata, plain_text);
        }
        if(check(plain_text)){
            if(trova(listaParole, plain_text)){
                printf("Chiave trovata: %s\n Parola: %s\nTentativi effettuati: %lld\n", chiave_provata,plain_text, tentativo);
                return 0;

            }
           
        }

        free(chiave_provata);
        free(plain_text);
    }
    return 0;
}