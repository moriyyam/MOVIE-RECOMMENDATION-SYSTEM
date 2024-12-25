#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <set>

using namespace std;

struct Movie {
    string title;
    string genre;
    double rating;
};

struct User {
    string username;
    set<string> preferences; // User's preferred genres
    set<string> watched;    // Movies the user has already watched
};

// Comparator for priority queue
struct Compare {
    bool operator()(const Movie& a, const Movie& b) {
        return a.rating < b.rating; // Higher rating movies have higher priority
    }
};

unordered_map<string, User> users;
vector<Movie> movies;

void loadMovies(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string title, genre;
        double rating;

        getline(ss, title, ',');
        getline(ss, genre, ',');
        ss >> rating;

        movies.push_back({title, genre, rating});
    }
    file.close();
}

void saveUserData(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for saving: " << filename << endl;
        return;
    }

    for (const auto& [username, user] : users) {
        file << username << "|";
        for (const auto& genre : user.preferences) {
            file << genre << ",";
        }
        file << "|";
        for (const auto& movie : user.watched) {
            file << movie << ",";
        }
        file << endl;
    }
    file.close();
}

void loadUserData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, prefStr, watchedStr;

        getline(ss, username, '|');
        getline(ss, prefStr, '|');
        getline(ss, watchedStr, '|');

        User user;
        user.username = username;

        stringstream prefSS(prefStr);
        string genre;
        while (getline(prefSS, genre, ',')) {
            if (!genre.empty()) user.preferences.insert(genre);
        }

        stringstream watchedSS(watchedStr);
        string movie;
        while (getline(watchedSS, movie, ',')) {
            if (!movie.empty()) user.watched.insert(movie);
        }

        users[username] = user;
    }
    file.close();
}

void recommendMovies(const string& username) {
    if (users.find(username) == users.end()) {
        cout << "User not found!" << endl;
        return;
    }

    User& user = users[username];
    priority_queue<Movie, vector<Movie>, Compare> recommendations;

    for (const auto& movie : movies) {
        if (user.watched.find(movie.title) == user.watched.end() && 
            user.preferences.find(movie.genre) != user.preferences.end()) {
            recommendations.push(movie);
        }
    }

    cout << "Recommended movies for " << username << ":\n";
    int count = 0;
    while (!recommendations.empty() && count < 5) {
        Movie movie = recommendations.top();
        recommendations.pop();
        cout << "- " << movie.title << " (" << movie.genre << ", Rating: " << movie.rating << ")" << endl;
        count++;
    }
}

int main() {
    string movieFile = "movies.csv";
    string userFile = "users.txt";

    loadMovies(movieFile);
    loadUserData(userFile);

    cout << "Welcome to the Movie Suggestion System!" << endl;
    string username;

    cout << "Enter your username: ";
    cin >> username;

    if (users.find(username) == users.end()) {
        cout << "New user detected. Setting up your profile." << endl;
        users[username] = {username};
        cout << "Enter your preferred genres (comma-separated): ";
        cin.ignore();
        string genreInput;
        getline(cin, genreInput);

        stringstream ss(genreInput);
        string genre;
        while (getline(ss, genre, ',')) {
            users[username].preferences.insert(genre);
        }
    }

    recommendMovies(username);

    saveUserData(userFile);

    return 0;
}
