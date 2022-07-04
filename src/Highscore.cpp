#include <fstream>

#include "Highscore.h"

void load_highscore(int& highscore) {
    std::ifstream highscore_file("assets/highscore.txt");
    if (!highscore_file)
        highscore = 0;
    else {
        highscore_file >> highscore;
        highscore_file.close();
    }
}

void save_highscore(const int highscore) {
    std::ofstream highscore_file;
    highscore_file.open ("assets/highscore.txt");
    highscore_file << highscore;
    highscore_file.close();
}