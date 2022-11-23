#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Spotify.h"
#include "Artista.h"
#include "Musica.h"
#include "Playlist.h"
#include "Propriedades.h"
#include "Menu.h"

struct Spotify
{
    p_Artista* vet_artistas;
        int art_allc; //quantidade de artistas alocados
        int art_qtd; //quantidade de artistas adicionados

    p_Musica* vet_musicas;
        int msc_allc; //quantidade de espacos de musicas alocados
        int msc_qtd; //quantidade de musicas adicionadas

    p_Playlist* vet_playlists;
        int pls_allc; //quantidade de espacos de playlists alocados
        int pls_qtd; //quantidade de playlists adicionadas

};

p_Spotify spotify_cria()
{
    //alocamento da TAD principal
    p_Spotify sp = (p_Spotify)calloc(1,sizeof(struct Spotify));
    
    //alocamento do Vetor de Artistas
    sp->art_allc = 500;
    sp->vet_artistas = (p_Artista *)calloc(1, sizeof(p_Artista)*sp->art_allc);

    //alocamento do Vetor de Musicas
    sp->msc_allc = 500;
    sp->vet_musicas = (p_Musica *)calloc(1, sizeof(p_Musica)*sp->msc_allc);

    //alocamento do Vetor de Playlists
    sp->pls_allc = 0;
    sp->vet_playlists = (p_Playlist *)calloc(1, sizeof(p_Playlist)*sp->pls_allc);

    return sp;
}

void spotify_inicia(p_Spotify spotify, char path[])
{
    arquivo_ler_artista_csv(spotify, path);
    arquivo_ler_musica_csv(spotify, path);

    OPCOES_DO_MENU opt;
    while (1)
    {
        opt = menu_show_options(spotify);

        if(opt == FINALIZAR)
        {
            break;
        }
    }
    
}

void arquivo_ler_artista_csv(p_Spotify spotify, char path[])
{
    char caminho[1000];
    sprintf(caminho,"%s/artists_full.csv", path);
    FILE* artcsv = fopen(caminho,"r");

    if(!artcsv)
    {
        printf("ERRO AO ABRIR ARQUIVO CSV DOS ARTISTAS\n");
        exit(1);
    }
    
    char linha[250];

    while (fscanf(artcsv,"%[^\n]\n",linha)!=EOF)
    {
        if(spotify->art_qtd == spotify->art_allc)
        {
            spotify->art_allc*=2;
            spotify->vet_artistas = (p_Artista*)realloc(spotify->vet_artistas,
                                                       (spotify->art_allc)*sizeof(p_Artista));
        }
        spotify->vet_artistas[spotify->art_qtd] = artista_cria();
        artista_le(spotify->vet_artistas[spotify->art_qtd],linha);
        spotify->art_qtd++;
    }
    
    fclose(artcsv);
}

void arquivo_ler_musica_csv(p_Spotify spotify, char path[]){
    char caminho[1000];
    sprintf(caminho,"%s/tracks_full.csv", path);
    FILE* msccsv = fopen(caminho,"r");

    if(!msccsv)
    {
        printf("ERRO AO ABRIR ARQUIVO CSV DAS MUSICAS\n");
        exit(1);
    }
    
    char linha[3000];
    
    while (fscanf(msccsv,"%[^\n]\n",linha)!=EOF)
    {
        if(spotify->msc_qtd == spotify->msc_allc)
        {
            spotify->msc_allc*=2;
            spotify->vet_musicas = (p_Musica*)realloc(spotify->vet_musicas,
                                                       (spotify->msc_allc)*sizeof(p_Musica));
        }
        spotify->vet_musicas[spotify->msc_qtd] = musica_cria();
        musica_le(spotify->vet_musicas[spotify->msc_qtd],linha);
        spotify->msc_qtd++;
        
    }
    
    fclose(msccsv);
}

void arquivo_ler_playlist_bin(char path[]);

void arquivo_salvar_playlist_bin(char path[]);

void spotify_busca_musica_titulo(p_Spotify spotify)
{
    char *str;
    char ch;
    int tam_text=0,tam_allc=30;
    printf("Digite o que deseja buscar em musicas: ");
    str = (char*)calloc(1,sizeof(char)*tam_allc);

    // While para ler o texto digitado caractere por caractere para
    // Possibilitar realocacao de espaco quando necessario
    scanf("\n%c", &ch);
    while (ch!='\n')
    {
        if((tam_text+1)==tam_allc)
        {
            tam_allc*=2;
            str = (char*)realloc(str,sizeof(char)*tam_allc);
        }

        str[tam_text] = ch;
        tam_text++;
        scanf("%c", &ch);
    }

    int i;
    // Loop que vai passar por todas as musicas do spotify
    for(i=0;i<spotify->msc_qtd;i++)
    {
        musica_busca_titulo(spotify->vet_musicas[i], str, i);
    }

    free(str);

    printf("\nPressione enter para voltar para o menu principal!");
    scanf("%*[^\n]%*c");
}

void spotify_lista_musica(p_Spotify spotify, int id)
{
    if(id<0 || id>=spotify->msc_qtd)
        printf("Entrada de indice invalida!");
    else
    {
        //Primeiro imprime os atributos da musica (com excessao do artista, que sera feito separadamente)
        musica_imprime_informacoes(spotify->vet_musicas[id], id);
        
        char **artistas_out; // Cria um um vetor de string que vai ser mudado por referencia para 
                             // receber o vetor de ids dos artistas das musicas
        
        int qtd_artistas;
        qtd_artistas = musica_retorna_id_artistas(spotify->vet_musicas[id], &artistas_out);

        int i = 0,j;
        int achou=0; // variavel booleana (0 ou 1) para cobrir casos em que o artista nao exista no artists.csv
        printf("|# Informacoes dos artistas:\n");
        // Loop que vai passar por todos os artistas da musica
        for(i=0;i<qtd_artistas;i++)
        {
            j=0;
            achou=0;
            // Loop que vai ficar rodando no vetor que contem todos os artistar ate achar o artista que tem aquele id
            while(!(artista_compara_id(spotify->vet_artistas[j], artistas_out[i], &achou)))
            {
                if(j==(spotify->art_qtd-1))
                    break;
                j++;
            }
            if(achou)
                artista_imprime(spotify->vet_artistas[j]);
            else
                musica_imprime_artista_inexistente(spotify->vet_musicas[id],i);
        }

        if(i==0)
        {
            printf("Essa musica nao possui nenhum artista registrado :/\n");
        }
    }

    printf("\nPressione enter para voltar para o menu principal!");
    fgetc(stdin);
    scanf("%*[^\n]%*c");
}

void spotify_playlist_cria(p_Spotify spotify){
    spotify->pls_qtd++;

    if(spotify->pls_qtd > spotify->pls_allc){
        spotify->pls_allc++;
        spotify->vet_playlists = (p_Playlist *)realloc(spotify->vet_playlists, sizeof(p_Playlist)*spotify->pls_allc);        
    }

    spotify->vet_playlists[spotify->pls_qtd-1] = playlist_cria();

    printf("\nPlaylist criada com sucesso!");

    printf("\n\nPressione enter para voltar para o menu principal!");
    scanf("%*[^\n]%*c");
}

void spotify_playlist_listar_todas(p_Spotify spotify){
    playlist_listar_todas(spotify->vet_playlists, spotify->pls_qtd);
}

void spotify_playlist_listar_uma(p_Spotify spotify){
    playlist_listar_uma(spotify->vet_playlists, spotify->pls_qtd, spotify->vet_musicas);
}

void spotify_playlist_adicionar_musica(p_Spotify spotify){
    playlist_adicionar_musica(spotify->vet_playlists, spotify->msc_qtd, spotify->pls_qtd);
}

void spotify_gerar_relatorio(p_Spotify spotify){

    if (spotify->pls_qtd == 0)
    {
        printf("Nao ha playlists!\n");
    }
    else
    {
        playlist_gerar_relatorio_musicas(spotify->vet_playlists, spotify->pls_qtd, spotify->vet_musicas);
        playlist_gerar_relatorio_artistas(spotify->vet_playlists, spotify->pls_qtd, spotify->vet_musicas);
    }
    printf("\nPressione enter para voltar para o menu principal!");
    fgetc(stdin);
    scanf("%*[^\n]%*c");
}

void spotify_recomendar_musicas(p_Spotify spotify)
{
    if(spotify->pls_qtd==0)
    {
        printf("Nao ha playlists!");
    }

    else
    {

        int indx_playlist;
        printf("Informe o indice da playlist a ser analisada para realizar a recomendacao: ");
        scanf("%d", &indx_playlist);

        while (indx_playlist<0 || indx_playlist>spotify->pls_qtd-1)
        {
            printf("\nNUMERO INVALIDO!\n\n");

            printf("Informe o indice da playlist a ser analisada para realizar a recomendacao: ");
            scanf("%d", &indx_playlist);
        }

        int qtd_recomendacoes;
        printf("Informe a quantidade de musicas a serem recomendadas: ");
        scanf("%d", &qtd_recomendacoes);

        while (qtd_recomendacoes<=0)
        {
            printf("\nNUMERO INVALIDO!\n\n");

            printf("Informe a quantidade de musicas a serem recomendadas: ");
            scanf("%d", &qtd_recomendacoes);
        }
        
        p_Propriedades med_playlist = playlist_retorna_media_propriedades(spotify->vet_playlists[indx_playlist], spotify->vet_musicas);
        p_Musica vet_musicas_selecionadas[qtd_recomendacoes], holder_msc; //holder eh uma variavel auxiliar que vai ajudar na ordenacao do vetor de musicas selecionadas
        double vet_distancias[qtd_recomendacoes], distancia, holder_dist;

        int i,j;

        //inicializa todos os elementos do vetor de distancia como -1, um valor artificial e impossivel para distancia euclidiana
        //afim de auxiliar no primeiro preenchimento do vetor de musicas selecionadas
        for(i=0;i<qtd_recomendacoes;i++)
        {
            vet_distancias[i]= -1; 
        }

        // for que vai percorrer todas as musicas do vetor
        // o vetor de musicas selecionadas sera ativamente ordenado de acordo com o vetor de distancias
        // o vetor de distancias vai sempre estar organizado em ordem cresncente, sendo que os primeiros elementos tem distancia menor que os ultimos
        // a unica excessao sera durante o primeiro preenchimento, ja que todos os valores do vetor serao -1
        // a ideia eh que os novos elementos sejam adicionados no fim do vetor SOMENTE SE a distancia for menor que a distancia atual do ultimo elemento do vetor (ou ter valor -1)
        // quando um elemento novo eh adicionado, ele sucessivamente trocara de posicoes com o elemento seguinte ate que o elemento seguinte seja menor (ou todos ate q n haja -1 na sua frente)
        // dessa forma eh garantido que a lista sempre estara ordenada de ordem crescente 
        // e musicas que tem distancia maior que o ultimo elemento do vetor sejam automaticamente descartadas
        for(i=0;i<spotify->msc_qtd;i++)
        {   
            //ignora musicas que ja estao na playlist
            if(playlist_verifica_se_esta_incluso(spotify->vet_playlists[indx_playlist],i))
                continue;

            distancia = propriedade_likeliness(med_playlist, musica_retorna_propriedade(spotify->vet_musicas[i]));

            // descarta distancias q sao maiores q a ultima distancia do vetor (exceto se for -1)
            if(distancia > vet_distancias[qtd_recomendacoes-1] && vet_distancias[qtd_recomendacoes-1] != -1)
                continue;

            // adiciona o novo elemento na ultima posicao do vetor
            vet_distancias[qtd_recomendacoes-1] = distancia;
            vet_musicas_selecionadas[qtd_recomendacoes-1] = spotify->vet_musicas[i];

            // loop que vai trocar o novo elemento adicionado de posicao ate que esteja devidamente posicionado 
            // afim de eliminar os -1 e manter a ordem crescente do vetor
            for(j=qtd_recomendacoes-1;j>=0;j--)
            {
                if(j==0) //se chegar no primeiro elemento
                    break;
                if(vet_distancias[j-1]<vet_distancias[j] && vet_distancias[j-1] != -1)
                    break;
                
                holder_dist = vet_distancias[j];
                holder_msc = vet_musicas_selecionadas[j];
                vet_distancias[j] = vet_distancias[j-1];
                vet_musicas_selecionadas[j] = vet_musicas_selecionadas[j-1];
                vet_distancias[j-1] = holder_dist;
                vet_musicas_selecionadas[j-1] = holder_msc;
            }
        }

        printf("Musicas recomendadas:\n\n");
        char **nome_out;
        int qtd_artistas_musica;
        for(i=0;i<qtd_recomendacoes;i++)
        {
            printf("> %s | ",musica_retorna_nome(vet_musicas_selecionadas[i]));
            qtd_artistas_musica = musica_retorna_nome_artistas(vet_musicas_selecionadas[i], &nome_out);
            for(j=0;j<qtd_artistas_musica;j++)
            {
                if(j==0)
                    printf("%s",nome_out[j]);
                else
                    printf(", %s",nome_out[j]);
            }
            printf("\n");
        }

        free(med_playlist);
    }
    printf("\nPressione enter para voltar para o menu principal!");
    fgetc(stdin);
    scanf("%*[^\n]%*c");
}

void spotify_destroi(p_Spotify spotify){
    for(int i = 0; i<spotify->art_qtd; i++)
    { 
        artista_destroi(spotify->vet_artistas[i]);
    }
    free(spotify->vet_artistas);
    
    for(int i = 0; i<spotify->msc_qtd; i++)
    { 
        musica_destroi(spotify->vet_musicas[i]);
    }
    free(spotify->vet_musicas);

    for(int i = 0; i<spotify->pls_qtd; i++)
    { 
        playlist_destroi(spotify->vet_playlists[i]);
    }
    free(spotify->vet_playlists);
    
    free(spotify);
}
