#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <limits>
#include <vector>
#include <cstdlib>
#include <iomanip>

using namespace std;

const string COLOR_RESET = "\033[0m";
const string COLOR_RED = "\033[31m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_CYAN = "\033[36m";

void clearScreen(){
    #ifdef _WIN32

    #else
        system("clear");
    #endif
}


string toLowerCase(const string &str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getIntegerInput(const string &prompt, int min = INT_MIN, int max = INT_MAX) {
    int value;
    while (true) {
        cout << COLOR_CYAN << prompt << COLOR_RESET;
        if (cin >> value && value >= min && value <= max) {
            clearInputBuffer();
            return value;
        }
        clearInputBuffer();
        cout << COLOR_RED << "Invalid input! Please enter between "
             << min << "-" << max << COLOR_RESET << endl;
    }
}

string getNonEmptyInput(const string &prompt){
    string input;
    while (true) {
        cout << COLOR_CYAN << prompt << COLOR_RESET;
        getline(cin, input);
        if (!input.empty()) return input;
        cout << COLOR_RED << "This field cannot be empty!" << COLOR_RESET << endl;
    }
}

class Flashcard {
private:
    long card_id;
    string category;
    string question;
    string answer;
    int progress;
    static long next_card_id;

public:
    Flashcard() : progress(0) {}
    Flashcard(string category, string question, string answer);

    long getCardID() const { return card_id; }
    string getCategory() const { return category; }
    string getQuestion() const { return question; }
    string getAnswer() const { return answer; }
    int getProgress() const { return progress; }


    void setCardID(long id) { card_id = id; }
    void setCategory(string new_category) { category = new_category; }
    void setQuestion(string new_question) { question = new_question; }
    void setAnswer(string new_answer) { answer = new_answer; }
    void setProgress(int p) { progress = p; }
    void incrementProgress() { if (progress < 5) progress++; }

    static void setLastCardID(long id);
    static long getLastCardID();

    friend ofstream &operator<<(ofstream &ofs, const Flashcard &card);
    friend ifstream &operator>>(ifstream &ifs, Flashcard &card);
    friend ostream &operator<<(ostream &os, const Flashcard &card);
};

long Flashcard::next_card_id = 0;

Flashcard::Flashcard(string category, string question, string answer)
    : category(category), question(question), answer(answer), progress(0) {
    card_id = ++next_card_id;
}

void Flashcard::setLastCardID(long id) {
    next_card_id = id;
}

long Flashcard::getLastCardID() {
    return next_card_id;
}

ofstream &operator<<(ofstream &ofs, const Flashcard &card) {
    ofs << card.card_id << '\n'
        << card.category << '\n'
        << card.question << '\n'
        << card.answer << '\n'
        << card.progress << '\n';
    return ofs;
}

ifstream &operator>>(ifstream &ifs, Flashcard &card) {
    ifs >> card.card_id;
    ifs.ignore();
    getline(ifs, card.category);
    getline(ifs, card.question);
    getline(ifs, card.answer);
    ifs >> card.progress;
    ifs.ignore();
    return ifs;
}

ostream &operator<<(ostream &os, const Flashcard &card) {
    os << COLOR_YELLOW << "ID: " << card.getCardID() << COLOR_RESET << '\n'
       << COLOR_BLUE << "Category: " << card.getCategory() << COLOR_RESET << '\n'
       << COLOR_CYAN << "Question: " << card.getQuestion() << COLOR_RESET << '\n'
       << COLOR_GREEN << "Answer: " << card.getAnswer() << COLOR_RESET << '\n'
       << "Progress: " << card.getProgress() << "/5\n";
    return os;
}

class FlashcardSystem {
private:
    map<long, Flashcard*> flashcards;

    void saveToFile() {
        try {
            ofstream outfile("Flashcards.data", ios::trunc);
            if (!outfile) throw runtime_error("Failed to save data!");

            for (const auto &pair : flashcards) {
                outfile << *pair.second;
            }
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

public:
    FlashcardSystem() {
        try {
            ifstream infile("Flashcards.data");
            if (!infile) return;

            Flashcard tempCard;
            long max_id = 0;
            while (infile >> tempCard) {
                if (tempCard.getCardID() > 0) {
                    Flashcard* newCard = new Flashcard(
                        tempCard.getCategory(),
                        tempCard.getQuestion(),
                        tempCard.getAnswer()
                    );
                    newCard->setCardID(tempCard.getCardID());
                    newCard->setProgress(tempCard.getProgress());
                    flashcards[newCard->getCardID()] = newCard;

                    if (newCard->getCardID() > max_id) {
                        max_id = newCard->getCardID();
                    }
                }
            }
            Flashcard::setLastCardID(max_id);
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error loading data: " << e.what() << COLOR_RESET << endl;
        }
    }

    ~FlashcardSystem() {
        for (auto& pair : flashcards) {
            delete pair.second;
        }
    }

    void addFlashcard(string category, string question, string answer) {
        try {
            Flashcard* newCard = new Flashcard(category, question, answer);
            flashcards[newCard->getCardID()] = newCard;
            saveToFile();
            cout << COLOR_GREEN << "Flashcard added successfully!\n" << COLOR_RESET;
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

    void updateFlashcard(long card_id, string new_category, string new_question, string new_answer) {
        try {
            if (flashcards.find(card_id) == flashcards.end()) {
                throw invalid_argument("Flashcard not found!");
            }
            Flashcard* card = flashcards[card_id];
            card->setCategory(new_category);
            card->setQuestion(new_question);
            card->setAnswer(new_answer);
            saveToFile();
            cout << COLOR_GREEN << "Flashcard updated successfully!\n" << COLOR_RESET;
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

    void deleteFlashcard(long card_id) {
        try {
            auto it = flashcards.find(card_id);
            if (it == flashcards.end()) {
                throw invalid_argument("Flashcard not found!");
            }
            delete it->second;
            flashcards.erase(it);
            saveToFile();
            cout << COLOR_GREEN << "Flashcard deleted successfully!\n" << COLOR_RESET;
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

    void showAllFlashcards() const {
        if (flashcards.empty()) {
            cout << COLOR_YELLOW << "No flashcards available!\n" << COLOR_RESET;
            return;
        }
        for (const auto &pair : flashcards) {
            cout << *pair.second << '\n';
        }
    }

    void practiceCategory(const string &category) {
        try {
            string lowerCategory = toLowerCase(category);
            vector<Flashcard*> sessionCards;
            int correct = 0, total = 0;

            for (auto &pair : flashcards) {
                if (toLowerCase(pair.second->getCategory()) == lowerCategory) {
                    sessionCards.push_back(pair.second);
                }
            }

            if (sessionCards.empty()) {
                throw invalid_argument("No cards in category: " + category);
            }

            clearScreen();
            cout << COLOR_BLUE << "Practicing: " << category
                 << " (" << sessionCards.size() << " cards)\n\n" << COLOR_RESET;

            for (auto card : sessionCards) {
                string answer;
                cout << COLOR_CYAN << "Question:\n" << card->getQuestion()
                     << COLOR_RESET << "\nYour answer: ";
                getline(cin, answer);

                if (toLowerCase(answer) == toLowerCase(card->getAnswer())) {
                    cout << COLOR_GREEN << "Correct!\n" << COLOR_RESET;
                    card->incrementProgress();
                    correct++;
                } else {
                    cout << COLOR_RED << "Wrong! Correct answer: "
                         << card->getAnswer() << COLOR_RESET << '\n';
                }
                total++;
            }

            saveToFile();
            cout << COLOR_BLUE << "\nSession Results: "
                 << COLOR_GREEN << correct << COLOR_BLUE << "/" << total
                 << " (" << fixed << setprecision(1)
                 << (static_cast<double>(correct)/total)*100 << "%)\n" << COLOR_RESET;
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

    void searchFlashcards(const string &keyword) const {
        try {
            string lowerKeyword = toLowerCase(keyword);
            vector<Flashcard*> results;

            for (const auto &pair : flashcards) {
                Flashcard* card = pair.second;
                if (toLowerCase(card->getQuestion()).find(lowerKeyword) != string::npos ||
                    toLowerCase(card->getAnswer()).find(lowerKeyword) != string::npos ||
                    toLowerCase(card->getCategory()).find(lowerKeyword) != string::npos) {
                    results.push_back(card);
                }
            }

            if (results.empty()){
                throw invalid_argument("No matches found for: " + keyword);
            }

            cout << COLOR_BLUE << "\nSearch Results (" << results.size() << " matches):\n" << COLOR_RESET;
            for (const auto card : results) {
                cout << *card << '\n';
            }
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }

    void showCategories() const{
        try {
            map<string, int> categoryStats;
            for (const auto &pair : flashcards) {
                categoryStats[pair.second->getCategory()]++;
            }

            if (categoryStats.empty()) {
                throw runtime_error("No categories available!");
            }

            cout << COLOR_BLUE << "\nCategories:\n" << COLOR_RESET;
            for (const auto &[category, count] : categoryStats) {
                cout << COLOR_YELLOW << left << setw(20) << category
                     << COLOR_RESET << ": " << COLOR_GREEN << count
                     << " cards\n" << COLOR_RESET;
            }
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
    }
};

void displayMainMenu() {
    clearScreen();
    cout << COLOR_BLUE << "INTERACTIVE FLASHCARD SYSTEM\n\n"
         << COLOR_CYAN << "1. Add New Flashcard\n"
         << "2. Edit Flashcard\n"
         << "3. Delete Flashcard\n"
         << "4. View All Flashcards\n"
         << "5. Practice Category\n"
         << "6. Search Flashcards\n"
         << "7. Show Categories\n"
         << COLOR_RED << "8. Exit\n\n"
         << COLOR_RESET;
}

int main(){
    FlashcardSystem system;
    while (true){
        displayMainMenu();
        int choice = getIntegerInput("Enter choice (1-8): ", 1, 8);

        try{
            switch (choice){
                case 1: {
                    string category = getNonEmptyInput("Category: ");
                    string question = getNonEmptyInput("Question: ");
                    string answer = getNonEmptyInput("Answer: ");
                    system.addFlashcard(category, question, answer);
                    break;
                }
                case 2: {
                    long id = getIntegerInput("Enter card ID: ", 1L, LONG_MAX);
                    system.updateFlashcard(id,
                        getNonEmptyInput("New category: "),
                        getNonEmptyInput("New question: "),
                        getNonEmptyInput("New answer: ")
                    );
                    break;
                }
                case 3: {
                    system.deleteFlashcard(getIntegerInput("Enter card ID: ", 1L, LONG_MAX));
                    break;
                }
                case 4:
                    system.showAllFlashcards();
                    break;
                case 5:
                    system.practiceCategory(getNonEmptyInput("Enter category: "));
                    break;
                case 6:
                    system.searchFlashcards(getNonEmptyInput("Search term: "));
                    break;
                case 7:
                    system.showCategories();
                    break;
                case 8:
                    clearScreen();
                    cout << COLOR_GREEN << "Thank you for using the Flashcard System!\n" << COLOR_RESET;
                    return 0;
                default:
                    cout << COLOR_RED << "Invalid choice!\n" << COLOR_RESET;
            }
        } catch (const exception &e) {
            cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        }
        cout << COLOR_CYAN << "\nPress enter to continue..." << COLOR_RESET;
        clearInputBuffer();
        getchar();
    }
}
