#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

typedef struct song{
  string artist;
  string name;
  int duration;

  song(string artist, string name, int duration){
    this->artist = artist;
    this->name = name;
    this->duration = duration;
  }
} song;

void add_song(list<song*>& songs){
  string artist, name, duration_string;
  cout << "Type artist: " << endl;
  getline(cin, artist);
  cout << "Type name: " << endl;
  getline(cin, name);
  cout << "Type duration in seconds: " << endl;
  getline(cin, duration_string);
  int duration = stoi(duration_string);
  song* new_song = new song(artist, name, duration);
  songs.push_back(new_song);
  sleep(2);

}

void remove_song(list<song*>& songs){
  int i = 0;
  map <int, list<song*>::iterator> index;
  for (auto it = songs.begin(); it != songs.end(); it ++, i++){
    cout << i+1 << "- " << "Artist: " << (*it)->artist << " Name: " << (*it)->name << " Duration: " << (*it)->duration << endl;
    index[i+1] = it;
  }
  sleep(2);
  if (songs.size() == 0 ) cout << "No songs to remove" << endl;
  else {
    cout << "Type the number of the song you want to remove" << endl;
    string index_remove_string;
    int index_remove;
    getline(cin, index_remove_string);
    index_remove = stoi(index_remove_string);
    songs.erase(index[index_remove]);
  }
  sleep(2);
}

void list_songs(list <song*>& songs){
  int i = 0;
  for (auto it = songs.begin(); it != songs.end(); it ++, i++){
    cout << i+1 << "- " << "Artist: " << (*it)->artist << " Name: " << (*it)->name << " Duration: " << (*it)->duration << endl;
  }
  if (songs.size() == 0) cout << "The list of songs is empty" << endl;
  sleep(2);
}

int main(){
  list <song*> songs;
  while (true){
    cout << "Menu" << endl;
    cout << "Add song- a" << endl;
    cout << "Remove song- r" << endl;
    cout << "List songs - l" << endl;
    cout << "Exit - e" << endl;
    string command;
    getline(cin, command);
    if (command == "a") add_song(songs);
    else if (command == "r") remove_song(songs);
    else if (command == "l") list_songs(songs);
    else if (command == "e") break;
    else cout << "Command not reconigzed, try anoter" << endl;
  }

}
