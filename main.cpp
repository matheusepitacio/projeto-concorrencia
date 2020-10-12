#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

typedef struct song
{
    string artist;
    string name;
    int duration;

    song(string artist, string name, int duration)
    {
        this->artist = artist;
        this->name = name;
        this->duration = duration;
    }

    song(){}
} song;

song* songs;
int size = 0;
int length = 0;
pthread_mutex_t mutex_thread = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread = PTHREAD_COND_INITIALIZER;

void * add_song(void* args)
{
    while (pthread_mutex_trylock(&mutex_thread));
    
    if (size == length){
        song* aux = new song [2*size + 1];
        for (int i = 0; i < size; i++) aux[i] = songs[i];

        delete [] songs;

        songs = aux;

        size = 2*size + 1;
    }

    song* new_song = (song* ) args;
    songs[length] = (*new_song);
    length++;


    for (int i = 0; i < length; i++) cout << i+1 << "- " << "Artist: " << songs[i].artist << " Name: " << songs[i].name << " Duration: " << songs[i].duration  << endl;


    pthread_cond_signal(&cond_thread);
    pthread_mutex_unlock(&mutex_thread);

    pthread_exit(NULL);
}

void* remove_song(void * args)
{
    while (pthread_mutex_trylock(&mutex_thread));

    int index = *(int*) args - 1;
    song* aux = new song[size];
    for (int i = 0; i < length; i++) if (i != index) aux[i] = songs[i];

    delete [] songs;
    songs = aux;
    length--;

    for (int i = 0; i < length; i++) cout << i+1 << "- " << "Artist: " << songs[i].artist << " Name: " << songs[i].name << " Duration: " << songs[i].duration  << endl;

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
            string artist, name, duration_string;
            cout << "Type artist: " << endl;
            getline(cin, artist);
            cout << "Type name: " << endl;
            getline(cin, name);
            cout << "Type duration in seconds: " << endl;
            getline(cin, duration_string);
            int duration = stoi(duration_string);
            song new_song (artist, name, duration);
            pthread_create(&add_thread, NULL, &add_song, &new_song);

            pthread_cond_wait(&cond_thread, &mutex_thread);

        }
        else if (command == "r")
        {
            pthread_t remove_thread;
            cout << "Type the number of the song you want to remove" << endl;
            string index_string;
            getline(cin, index_string);
            int index = stoi(index_string);
            pthread_create(&remove_thread, NULL, &remove_song, &index);

            pthread_cond_wait(&cond_thread, &mutex_thread);
        }
        else if (command == "e") break;
        else cout << "Command not reconigzed, try another" << endl;
   
    }
}
