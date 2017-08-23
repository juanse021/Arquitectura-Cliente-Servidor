#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <zmqpp/zmqpp.hpp>


using namespace std;
using namespace zmqpp;
using namespace sf;

void messageToFile(const message &msg, const string &fileName) {
    const void *data;
    msg.get(&data, 1);
    size_t size = msg.size(1);

    ofstream ofs(fileName, ios::binary);
    ofs.write((char *)data, size);
}

void threadSong(Music *music) {
    while (true) {
        while (music->getStatus() == SoundSource::Playing) {
            continue;
        }
    }
}

int main() {

    cout << "This is the client" << endl;
    context ctx;
    socket s(ctx, socket_type::req);
    cout << "Connecting to tcp port 5555" << endl;
    s.connect("tcp://localhost:5555");

    Music music;
    thread t1(threadSong, &music);

    while (true) {

        cout << "Option? " << endl;
        string option;
        cin >> option;

        message n;
        n << option;


        // Send namesong to play
        if (option == "play") {
            string song;
            cout << "Name song: ";
            cin >> song;
            n << song;
        }

        s.send(n);

        // Receive operation to do
        message answer;
        s.receive(answer);        
        string operation;
        answer >> operation;


        // List all available songs
        if (operation == "list") {
            size_t numSongs;
            string songName;
            answer >> numSongs;
            cout << "Available songs: " << numSongs << endl;

            for (int i = 0; i < int(numSongs); i++) {
                answer >> songName;
                cout << (i+1) << ":\t" << songName << endl;
            }
        }

        // Download and play song
        else if (operation == "file") {
            messageToFile(answer, "song.ogg");
            music.openFromFile("song.ogg");
            music.play();    
        } 

        else {
            cout << "Don't know what to do!!!" << endl;
        }
    }

    return 0;
}