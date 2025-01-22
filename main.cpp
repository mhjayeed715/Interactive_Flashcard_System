#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

string toLowerCase(const string &str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
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

    long getCardID() { return card_id; }
    string getCategory() { return category; }
    string getQuestion() { return question; }
    string getAnswer() { return answer; }
    int getProgress() { return progress; }

    void setCategory(string new_category) { category = new_category; }
    void setQuestion(string new_question) { question = new_question; }
    void setAnswer(string new_answer) { answer = new_answer; }
    void incrementProgress() { progress++; }

    static void setLastCardID(long id);
    static long getLastCardID();

    friend ofstream &operator<<(ofstream &ofs, Flashcard &card);
    friend ifstream &operator>>(ifstream &ifs, Flashcard &card);
    friend ostream &operator<<(ostream &os, Flashcard &card);
};

long Flashcard::next_card_id = 0;

Flashcard::Flashcard(string category, string question, string answer)
    : category(category), question(question), answer(answer), progress(0) {
    next_card_id++;
    card_id = next_card_id;
}

void Flashcard::setLastCardID(long id) {
    next_card_id = id;
}

long Flashcard::getLastCardID() {
    return next_card_id;
}

ofstream &operator<<(ofstream &ofs, Flashcard &card) {
    ofs << card.card_id << endl;
    ofs << card.category << endl;
    ofs << card.question << endl;
    ofs << card.answer << endl;
    ofs << card.progress << endl;
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

ostream &operator<<(ostream &os, Flashcard &card) {
    os << "Card ID: " << card.getCardID() << endl;
    os << "Category: " << card.getCategory() << endl;
    os << "Question: " << card.getQuestion() << endl;
    os << "Answer: " << card.getAnswer() << endl;
    os << "Progress: " << card.getProgress() << endl;
    return os;
}

class FlashcardSystem {
private:
    map<long, Flashcard> flashcards;

public:
    FlashcardSystem();
    Flashcard addFlashcard(string category, string question, string answer);
    Flashcard getFlashcard(long card_id);
    void updateFlashcard(long card_id, string new_category, string new_question, string new_answer);
    void deleteFlashcard(long card_id);
    void showAllFlashcards();
    void practiceFlashcards(string category);
    void searchFlashcards(string keyword);
    void filterByCategory(string category);
    ~FlashcardSystem();
};

FlashcardSystem::FlashcardSystem() {
    Flashcard card;
    ifstream infile("Flashcards.data");
    if (!infile) {
        return;
    }
    while (!infile.eof()) {
        infile >> card;
        if (infile.eof()) break;
        flashcards.insert(pair<long, Flashcard>(card.getCardID(), card));
    }
    Flashcard::setLastCardID(card.getCardID());
    infile.close();
}

Flashcard FlashcardSystem::addFlashcard(string category, string question, string answer) {
    Flashcard card(category, question, answer);
    flashcards.insert(pair<long, Flashcard>(card.getCardID(), card));

    ofstream outfile("Flashcards.data", ios::trunc);
    for (auto &pair : flashcards) {
        outfile << pair.second;
    }
    outfile.close();

    return card;
}

Flashcard FlashcardSystem::getFlashcard(long card_id) {
    if (flashcards.find(card_id) == flashcards.end()) {
        throw invalid_argument("Flashcard not found.");
    }
    return flashcards[card_id];
}

void FlashcardSystem::updateFlashcard(long card_id, string new_category, string new_question, string new_answer) {
    if (flashcards.find(card_id) == flashcards.end()) {
        throw invalid_argument("Flashcard not found.");
    }
    flashcards[card_id].setCategory(new_category);
    flashcards[card_id].setQuestion(new_question);
    flashcards[card_id].setAnswer(new_answer);

    ofstream outfile("Flashcards.data", ios::trunc);
    for (auto &pair : flashcards) {
        outfile << pair.second;
    }
    outfile.close();
}

void FlashcardSystem::deleteFlashcard(long card_id) {
    if (flashcards.find(card_id) == flashcards.end()) {
        throw invalid_argument("Flashcard not found.");
    }
    flashcards.erase(card_id);

    ofstream outfile("Flashcards.data", ios::trunc);
    for (auto &pair : flashcards) {
        outfile << pair.second;
    }
    outfile.close();

    cout << "Flashcard Deleted Successfully!" << endl;
}

void FlashcardSystem::showAllFlashcards() {
    if (flashcards.empty()) {
        cout << "\nNo flashcards available to display!\n";
        return;
    }

    for (auto &pair : flashcards) {
        cout << pair.second << endl;
    }
}

void FlashcardSystem::practiceFlashcards(string category) {
    string lowerCategory = toLowerCase(category);
    bool found = false;

    for (auto &pair : flashcards) {
        if (toLowerCase(pair.second.getCategory()) == lowerCategory) {
            string userAnswer;
            cout << "\nQuestion: " << pair.second.getQuestion() << endl;
            cout << "Your Answer: ";
            getline(cin, userAnswer);

            string actualAnswer = toLowerCase(pair.second.getAnswer());
            string lowerUserAnswer = toLowerCase(userAnswer);

            if (lowerUserAnswer == actualAnswer) {
                cout << "Correct!\n";
                pair.second.incrementProgress();
            } else {
                cout << "Wrong! The correct answer is: " << pair.second.getAnswer() << "\n";
            }

            found = true;
        }
    }

    if (!found) {
        cout << "No flashcards found in the category: " << category << endl;
    }
}

void FlashcardSystem::searchFlashcards(string keyword) {
    string lowerKeyword = toLowerCase(keyword);
    bool found = false;

    for (auto &pair : flashcards) {
        if (toLowerCase(pair.second.getQuestion()).find(lowerKeyword) != string::npos ||
            toLowerCase(pair.second.getCategory()).find(lowerKeyword) != string::npos) {
            cout << pair.second << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No flashcards found with the keyword: " << keyword << endl;
    }
}

void FlashcardSystem::filterByCategory(string category) {
    string lowerCategory = toLowerCase(category);
    bool found = false;

    for (auto &pair : flashcards) {
        if (toLowerCase(pair.second.getCategory()) == lowerCategory) {
            cout << pair.second << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No flashcards found in the category: " << category << endl;
    }
}

FlashcardSystem::~FlashcardSystem() {
    ofstream outfile("Flashcards.data", ios::trunc);
    for (auto &pair : flashcards) {
        outfile << pair.second;
    }
    outfile.close();
}

int main() {
    FlashcardSystem system;
    int choice;

    do {
        cout << "\nINTERACTIVE FLASHCARD SYSTEM" << endl;
        cout << "1. Add Flashcard" << endl;
        cout << "2. Update Flashcard" << endl;
        cout << "3. Delete Flashcard" << endl;
        cout << "4. Show All Flashcards" << endl;
        cout << "5. Practice Flashcards by Category" << endl;
        cout << "6. Search Flashcards" << endl;
        cout << "7. Filter Flashcards by Category" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            string category, question, answer;
            cout << "Enter Category: ";
            getline(cin, category);
            cout << "Enter Question: ";
            getline(cin, question);
            cout << "Enter Answer: ";
            getline(cin, answer);
            system.addFlashcard(category, question, answer);
            break;
        }
        case 2: {
            long card_id;
            string new_category, new_question, new_answer;
            cout << "Enter Flashcard ID to Update: ";
            cin >> card_id;
            cin.ignore();
            cout << "Enter New Category: ";
            getline(cin, new_category);
            cout << "Enter New Question: ";
            getline(cin, new_question);
            cout << "Enter New Answer: ";
            getline(cin, new_answer);
            try {
                system.updateFlashcard(card_id, new_category, new_question, new_answer);
                cout << "Flashcard Updated Successfully!" << endl;
            } catch (invalid_argument &e) {
                cout << e.what() << endl;
            }
            break;
        }
        case 3: {
            long card_id;
            cout << "Enter Flashcard ID to Delete: ";
            cin >> card_id;
            cin.ignore();
            try {
                system.deleteFlashcard(card_id);
            } catch (invalid_argument &e) {
                cout << e.what() << endl;
            }
            break;
        }
        case 4:
            system.showAllFlashcards();
            break;
        case 5: {
            string category;
            cout << "Enter Category to Practice: ";
            getline(cin, category);
            system.practiceFlashcards(category);
            break;
        }
        case 6: {
            string keyword;
            cout << "Enter Keyword to Search: ";
            getline(cin, keyword);
            system.searchFlashcards(keyword);
            break;
        }
        case 7: {
            string category;
            cout << "Enter Category to Filter: ";
            getline(cin, category);
            system.filterByCategory(category);
            break;
        }
        case 8:
            cout << "Exiting the system..." << endl;
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 8);

    return 0;
}
