
#include <stdio.h>
#include <string.h>
#define numcontador 1

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

#define QR(a, b, c, d)             \
    a += b, d ^= a, d = ROTL(d,16), \
    c += d, b ^= c, b = ROTL(b,12), \
    a += b, d ^= a, d = ROTL(d, 8), \
    c += d, b ^= c, b = ROTL(b, 7)  \

//0x61707865

//

void rodada(long *rod,long *chave,long *contadoronce){
    int x;
    const long constantes[4]={'expa','nd 3','2-by','te k'};

    long matchacha[16]={constantes[0],constantes[1],constantes[2],constantes[3],
    chave[0],chave[1],chave[2],chave[3],
    chave[4],chave[5],chave[6],chave[7],
    contadoronce[0],contadoronce[1],contadoronce[2],contadoronce[3]};

    long matchachaini[16]={constantes[0],constantes[1],constantes[2],constantes[3],
    chave[0],chave[1],chave[2],chave[3],
    chave[4],chave[5],chave[6],chave[7],
    contadoronce[0],contadoronce[1],contadoronce[2],contadoronce[3]};

    for(x=0; x<10;x++){
       QR(matchacha[0],matchacha[4],matchacha[8],matchacha[12]);
       QR(matchacha[1],matchacha[5],matchacha[9],matchacha[13]);
       QR(matchacha[2],matchacha[6],matchacha[10],matchacha[14]);
       QR(matchacha[3],matchacha[7],matchacha[11],matchacha[15]);

       QR(matchacha[0],matchacha[5],matchacha[10],matchacha[15]);
       QR(matchacha[1],matchacha[6],matchacha[11],matchacha[12]);
       QR(matchacha[2],matchacha[7],matchacha[8],matchacha[13]);
       QR(matchacha[3],matchacha[4],matchacha[9],matchacha[14]);
    }
    for (x = 0; x < 16; x++)
    {
        rod[x]=matchacha[x]+matchachaini[x];
    }
    
}
void chacha20 (char *texto,int tamtexto,char *textoEncDec,long *chave,long *nonce){
    long rod[16];
    long contador[numcontador];
    char rodB[64];
    int x,y;
    long contadoronce[4]={contador[0],(numcontador==2? nonce[2]:contador[1]),nonce[1],nonce[0]};

    for (x=0 ; x< tamtexto; x+=64){  
        contadoronce[0] +=1;
        if (numcontador==1) contadoronce[1] +=2; 
        rodada(rod,chave,contadoronce);
        memcpy(rodB, rod, sizeof(rod));
        for (y=0;y<64 && (x+y)<tamtexto;y++){
            textoEncDec[x + y] = texto[x + y] ^ rodB[y];
        }
    }
}



int main (){
    long chave[8]={0};
    long nonce[4-numcontador]={0};
    
    
    char textoSecreto[]="Teste algoritimo chacha20 ";
    char textoEncDec[sizeof(textoSecreto)];
    int tamtexto = sizeof(textoSecreto) - 1;
    printf("Texto Entrada : %s\n",textoSecreto);
    chacha20 (textoSecreto, tamtexto,textoEncDec,chave,nonce);
        printf("Texto Encriptado : %s\n",textoEncDec);
       
        chacha20 (textoEncDec, tamtexto,textoSecreto,chave,nonce);
         printf("Texto Desencriptado : %s",textoSecreto);
    return 0;

}