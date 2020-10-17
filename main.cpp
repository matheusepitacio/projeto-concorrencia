#include <bits/stdc++.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

typedef struct Play_song_args
{
    string song;
    int duration;
    WINDOW *song_win;
} Play_song_args;

typedef struct Song
{
    string song_name;
    int duration;
} Song;

int maxY = 0, maxX = 0;

pthread_mutex_t mutex_add = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_add = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_remove = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_remove = PTHREAD_COND_INITIALIZER;
Song *songs = new Song[1];
int size = 1;
int length = 0;

void *add_song(void *args)
{
    while (pthread_mutex_trylock(&mutex_add))
        ;

    Song *song = (Song *)args;
    if (size == length)
    {
        Song *aux = new Song[2 * size];
        for (int i = 0; i < size; i++)
        {
            aux[i] = songs[i];
        }

        delete[] songs;

        songs = aux;

        size = 2 * size;
    }

    songs[length] = (*song);
    length++;

    pthread_cond_signal(&cond_add);
    pthread_mutex_unlock(&mutex_add);

    pthread_exit(NULL);
}

void *remove_song(void *args)
{
    while (pthread_mutex_trylock(&mutex_remove))
        ;

    int index = *(int *)args;
    if (index >= 0 && index < length)
    {
        for (int i = index; i < length; ++i)
        {
            songs[i] = songs[i + 1];
        }

        length--;
    }

    pthread_cond_signal(&cond_remove);
    pthread_mutex_unlock(&mutex_remove);

    pthread_exit(NULL);
}

int pause_song = 0;
int stop_song = 0;
bool song_playing = false;
int next_song = 0;

void *play_song(void *args)
{
    int choice = *(int *) args;
    WINDOW *song_win = newwin(maxY / 2, maxX / 2, maxY / 2, 0);
    refresh();
    box(song_win, 0, 0);
    mvwprintw(song_win, 0, (maxX / 2 - strlen("Song")) / 2, "Song");
    wrefresh(song_win);
    song_playing = true;
    stop_song = 0;
    

    
    while (true)
    {
        Song song; 
        if (choice < length){
            song = songs[choice];
        } else {
            song = songs[0];
            choice = 0;
        }
        
        int time = 0;
        mvwprintw(song_win, 1, 1, "Song: %s", song.song_name.c_str());
        while (time <= song.duration)
        {
            if (pause_song == 0)
            {
                mvwprintw(song_win, 2, 1, "%d/%d", time, song.duration);
                wrefresh(song_win);
                napms(1000);
                time++;
            }
            if (stop_song == 1)
                break;
            if (next_song == 1){
                break;
            }
        }
        next_song = 0;
        choice++;
        if (stop_song == 1) break;
    }

    pause_song = 0;
    stop_song = 0;

    wclear(song_win);
    werase(song_win);

    pthread_exit(NULL);
}

int main()
{
    initscr();
    cbreak();
    noecho();

    getmaxyx(stdscr, maxY, maxX);

    WINDOW *playlist_win = newwin(maxY / 2, maxX, 0, 0);

    WINDOW *menu_win = newwin(maxY / 2, maxX / 2, maxY / 2, maxX / 2);
    refresh();
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, (maxX / 2 - strlen("Menu")) / 2, "Menu");
    mvwprintw(menu_win, 1, 1, "Add song - a");
    mvwprintw(menu_win, 2, 1, "Delete song - a");
    mvwprintw(menu_win, 3, 1, "Play song - p");
    mvwprintw(menu_win, 4, 1, "Pause/return song - s");
    mvwprintw(menu_win, 5, 1, "Next song - n");
    mvwprintw(menu_win, 6, 1, "Exit - e");
    wrefresh(playlist_win);
    wrefresh(menu_win);

    keypad(playlist_win, true);
    int highlight = 0;
    int range_begin = 0;
    int range_end = maxY / 2 - 2;

    wclear(playlist_win);
    box(playlist_win, 0, 0);
    mvwprintw(playlist_win, 0, (maxX - strlen("Playlist")) / 2, "Playlist");

    while (true)
    {
        wrefresh(playlist_win);
        for (int i = range_begin; i < range_end; i++)
        {
            if (i < length)
            {
                if (i == highlight)
                    wattron(playlist_win, A_REVERSE);
                mvwprintw(playlist_win, i + 1 - range_begin, 1, "Song: %s, duration: %d seconds", songs[i].song_name.c_str(), songs[i].duration);
                wattroff(playlist_win, A_REVERSE);
            }
        }
        wrefresh(playlist_win);

        int command = wgetch(playlist_win);

        Song song;
        string song_name;
        string duration_string;
        int duration;

        switch (command)
        {
        case KEY_UP:
            highlight--;
            if (highlight < range_begin)
                range_begin--, range_end--;
            if (highlight < 0)
                highlight = 0;
            if (range_begin < 0)
                range_begin++, range_end++;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight >= range_end && length >= 10)
                range_begin++, range_end++;
            if (highlight >= length)
                highlight = length - 1;
            if (range_end > length && length >= 10)
                range_begin--, range_end--;
            break;
        case 'a':
            wclear(playlist_win);

            box(playlist_win, 0, 0);
            mvwprintw(playlist_win, 0, (maxX - strlen("Add song")) / 2, "Add song");
            mvwprintw(playlist_win, 1, 1, "Type the name of the song ");
            wrefresh(playlist_win);
            while (true)
            {
                int c = wgetch(playlist_win);
                if (c == ERR || c == '\n')
                    break;
                else if (c == KEY_BACKSPACE)
                    song_name = song_name.substr(0, song_name.size() - 1);
                else
                    song_name = song_name + (char)c;
                mvwprintw(playlist_win, 1, 1 + strlen("Type the name of the song "), song_name.c_str());
                wrefresh(playlist_win);
            }
            mvwprintw(playlist_win, 2, 1, "Type the duration of the song in seconds ");
            while (true)
            {
                int c = wgetch(playlist_win);
                if (c == ERR || c == '\n')
                    break;
                else if (c == KEY_BACKSPACE)
                    duration_string = duration_string.substr(0, duration_string.size() - 1);
                else if (c >= '0' && c <= '9')
                    duration_string = duration_string + char(c);
                mvwprintw(playlist_win, 2, 1 + strlen("Type the duration of the song in seconds "), duration_string.c_str());
            }
            duration = stoi(duration_string);

            song.song_name = song_name;
            song.duration = duration;

            pthread_t add_thread;
            pthread_create(&add_thread, NULL, &add_song, &song);
            pthread_cond_wait(&cond_add, &mutex_add);
            wclear(playlist_win);
            box(playlist_win, 0, 0);
            mvwprintw(playlist_win, 0, (maxX - strlen("Playlist")) / 2, "Playlist");
            break;
        case 'd':

            pthread_t remove_thread;
            pthread_create(&remove_thread, NULL, &remove_song, &highlight);
            pthread_cond_wait(&cond_remove, &mutex_remove);

            wclear(playlist_win);
            box(playlist_win, 0, 0);
            mvwprintw(playlist_win, 0, (maxX - strlen("Playlist")) / 2, "Playlist");

            highlight = 0;
            break;
        case 'p':
            stop_song = 1;
            sleep(1);
            pthread_t song_thread;

            pthread_create(&song_thread, NULL, &play_song, &highlight);
            break;
        case 's':
            pause_song = abs(pause_song - 1);
            break;
        case 'x':
            stop_song = 1;
            break;
        case 'n':
            next_song = 1;
            break;
        }
    }

    endwin();
}
