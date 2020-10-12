#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

typedef struct song //struct da musica
{
    string artist;
    string name;
    int duration;

    song(string artist, string name, int duration) //construtor com argumentos
    {
        this->artist = artist;
        this->name = name;
        this->duration = duration;
    }

    song() {} //construtor sem argumentos
} song;

song *songs = new song[1];                                              //array dinamico que guarda a lista de musicas
int size = 1;                                             //tamanho fisico
int length = 0;                                           //tamanho logico
pthread_mutex_t mutex_thread = PTHREAD_MUTEX_INITIALIZER; //mutex
pthread_cond_t cond_thread = PTHREAD_COND_INITIALIZER;    //condicional

void *add_song(void *args) //funcao para adicionar musica, o argumento e um objeto do tipo song
{
    while (pthread_mutex_trylock(&mutex_thread))
        ; //mutex para proteger o songs

    if (size == length)
    { // funcao que dobra o tamanho do array dinamico
        song *aux = new song[2 * size];
        for (int i = 0; i < size; i++)
            aux[i] = songs[i];

        delete[] songs;

        songs = aux;

        size = 2 * size;
    }

    song *new_song = (song *)args;
    songs[length] = (*new_song);
    length++;

    for (int i = 0; i < length; i++)
        cout << i + 1 << "- "
             << "Artist: " << songs[i].artist << " Name: " << songs[i].name << " Duration: " << songs[i].duration << endl;
    //printando a lista de musicas

    pthread_cond_signal(&cond_thread);   //liberando a condicao
    pthread_mutex_unlock(&mutex_thread); //desprotegendo a variavel

    pthread_exit(NULL);
}

void *remove_song(void *args) //funcao para remocao de musicas, o argumento e um inteiro que indica o indice de musica a ser removida
{
    while (pthread_mutex_trylock(&mutex_thread));

    int index = *(int *)args - 1;
    if (index >= 0 && index < length) //checka se o index a removido esta dentro do array
    {
        for (int i = index; i < length; ++i)
            songs[i] = songs[i + 1];
        length--;

        for (int i = 0; i < length; i++)
            cout << i + 1 << "- "
                 << "Artist: " << songs[i].artist << " Name: " << songs[i].name << " Duration: " << songs[i].duration << endl;
        //printando a lista de musicas
    } else {
        cout << "Index out of range" << endl;
    }

    pthread_cond_signal(&cond_thread);
    pthread_mutex_unlock(&mutex_thread);

    pthread_exit(NULL);
}

int main()
{
    cout << "Menu" << endl;
    cout << "Add song- a" << endl;
    cout << "Remove song- r" << endl;
    cout << "List songs - l" << endl;
    cout << "Exit - e" << endl;
    while (true)
    {
        string command;
        getline(cin, command);
        if (command == "a")
        {
            pthread_t add_thread; 
            string artist, name, duration_string; //le a linha de cada entrada
            cout << "Type the artist of the song: " << endl;
            getline(cin, artist);
            cout << "Type the name of the song: " << endl;
            getline(cin, name);
            cout << "Type the duration of the song in seconds: " << endl;
            getline(cin, duration_string);
            int duration = stoi(duration_string); //converte a linha da duracao pra int
            song new_song(artist, name, duration); 
            pthread_create(&add_thread, NULL, &add_song, &new_song);

            pthread_cond_wait(&cond_thread, &mutex_thread);
        }
        else if (command == "r")
        {
            pthread_t remove_thread;
            cout << "Type the index of the song to be removed" << endl;
            string index_string;
            getline(cin, index_string);
            int index = stoi(index_string); //converte a linha do indice para int
            pthread_create(&remove_thread, NULL, &remove_song, &index);

            pthread_cond_wait(&cond_thread, &mutex_thread);
        }
        else if (command == "e")
            break;
        else
            cout << "Command not reconigzed, try another" << endl;
    }
}
