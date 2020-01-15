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
/**Potenza su numberi interi*/
long long pown(short int base, short int esponente){
    long long potenza=1;
    for(int i = 0; i<esponente; i++){
        potenza = potenza *base;
    }
    return potenza;

}

/*  DISPOSIZIONI CON RIPETIZIONE IN 8 POSTI CON 28 ELEMENTI
    RITORNA LA DISPOSIZIONE ALLA POSIZIONE N  
    USATA PER IL CALCOLO DELLA DISPOSIZIONE INIZIALE     */
void combo(char* combo, char* charset, long long  N){
    long long potenza;
    int pos;
    
    //Calcolo della disposizione
    for(int c=7; c>=0; c--){
        if(N!=0){
            potenza = pown(28,c);
            if(N>=potenza){
                pos = N/potenza;
                
                combo[7-c]= charset[pos];
                N = N % potenza;
            }
            else{
                combo[7-c]= charset[0];
            }
        }
        else{
            combo[7-c]= charset[0];
        }
    }
}


/* DECRYPT DES */
void Decrypt(char* plain_text,char* key,char*  cipher_text){
    DES_cblock key2;
    DES_key_schedule schedule;
    //Prepare the key
    memcpy(key2, key, 8);
    DES_set_odd_parity(&key2);
    DES_set_key_checked(&key2, &schedule);
    // Decrypt ECB
    DES_ecb_encrypt((unsigned char (*)[8]) cipher_text, (unsigned char (*)[8]) plain_text, &schedule, DES_DECRYPT);
   

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
/**
 * RICERCA BINARIA
 * ritorna la posizione dell' elemento trovato
 */
int RicercaBinaria(char key, char* charset, int len){
    int start = 0;
    int end = len  ;
    int pos_mid= end /2;
    while(true){
        if(key ==charset[pos_mid]){
            return pos_mid;
        }
        else if(key>charset[pos_mid]){
            start= pos_mid;
            pos_mid = (end + start)/2;
        }
        else if(key<charset[pos_mid]){
            end = pos_mid;
            pos_mid = (end+start)/2;
        }

    }


}
   
/**
 * Sale di un posto rispetto alla precedente disposizione
 * ritorna una stringa che rappresenta la disposizione alla posizione successiva  
 */

void sali(char* chiave_provata, char* charset){
    int pos;
    for(int i=7; i>=0; i--){
        pos = RicercaBinaria(chiave_provata[i],charset, 28);
        pos =(pos +1) %28;
        chiave_provata[i] = charset[pos];
        if(pos != 0 ){ break;}
        
    }
}

    

int main(int argc, char* argv[]){
    char charset[29] = {'A','B','D','F','H','J','L','N','P','R','T','V','X','Z','a','b','d','f','h','j','l','n','p','r','t','v','x','z', '\0'};
    char * cipher_text;
    char* chiave_provata = (char*)malloc(8*sizeof(char));//SPAZIO ALLOCATO PER LE COMBINAZIONI
    char* plain_text = (char*)malloc(8*sizeof(char));//SPAZIO PER IL PLAIN TEXT
    //START SETTINGS
    long long START;
    if(argc==2){
        START = atoll(argv[1]);
    }
    else{
        START = 0;
    }
    //DISPOSIZIONI INIZIALE DA CUI PARTIRE
    combo(chiave_provata, charset, START);
    //SHOW CHARSET
    printf("%s\n", charset);
    //LOAD CIPHERTEXT
    cipher_text = (char*)malloc(8*sizeof(char));
    memcpy(cipher_text, "v\x98\xb8s\xe3Hg\x13", 8); // Cipher_text della challenge
    // LOAD LIST OF WORDS
    std::list <char* > listaParole;
    load(&listaParole);
    // TENTATIVI
    for(long long tentativo = START; tentativo<=300000000000;tentativo++){
        Decrypt(plain_text, chiave_provata, cipher_text);
        if(tentativo%100000000==0){ 
            printf("Tentativo: %lld  Chiave provata: %s Plain text: %s\n",tentativo ,chiave_provata, plain_text);
        }
        if(check(plain_text)){
            if(trova(listaParole, plain_text)){
                printf("Chiave trovata: %s\n Parola: %s\nTentativi effettuati: %lld\n", chiave_provata,plain_text, tentativo);
                return 0;

            }
           
        }
        //RICALCOLO CHIAVE (chiave_provata = chiave_provata +1)
        sali(chiave_provata, charset);

    }
    return 0;
}