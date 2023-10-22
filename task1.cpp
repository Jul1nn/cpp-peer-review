#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

class Readers {
public:
    void Read(int user, int page) {
        if (users_to_pages_.size() < (user + 1)) {
            users_to_pages_.resize(user + 1, 0);
            ++users_count_;
        }
        else
            if (users_to_pages_[user] == 0) {
                ++users_count_;
            }
        if (pages_to_users_.size() < (page + 1)) {
            pages_to_users_.resize(page + 1, 0);
        }
        for (int i = users_to_pages_[user] + 1; i <= page; ++i) {
            pages_to_users_[i] += 1;
        }
        users_to_pages_[user] = page;
    }

    void Cheer(int user) {
        if (users_to_pages_.size() < (user + 1) || users_to_pages_[user] == 0) {
            cout << 0 << endl;
        }
        else if (users_to_pages_[user] > 0 && users_count_ == 1) {
            cout << 1 << endl;
        }
        else {
            double part = 1.0 - 1.0 * (pages_to_users_[users_to_pages_[user]] - 1) / (users_count_ - 1);
            cout << setprecision(6) << part << endl;
        }
    }

private:
    int users_count_ = 0;
    vector<int> users_to_pages_;
    vector<int> pages_to_users_;
};

int main() {
    Readers readers;
    int Q;
    cin >> Q;
    for (int i = 0; i < Q; ++i) {
        string s;
        cin >> s;
        if (s == "READ"s) {
            int user, page;
            cin >> user;
            cin >> page;
            readers.Read(user, page);
        }
        if (s == "CHEER"s) {
            int user;
            cin >> user;
            readers.Cheer(user);
        }
    }
}