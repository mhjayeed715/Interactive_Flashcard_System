# Interactive Flashcard System  

The **Interactive Flashcard System** is a console-based learning application developed in **C++**. It allows users to create, manage, and practice flashcards for effective information retention. The system provides several features like categorization, progress tracking, and case-insensitive searching to enhance the user experience.  

## Features  
1. **Add Flashcard**:  
   - Create flashcards with a `Category`, `Question`, and `Answer`.  
   - Automatically assigns a unique ID to each flashcard.  

2. **Update Flashcard**:  
   - Modify the `Category`, `Question`, or `Answer` of an existing flashcard by ID.  

3. **Delete Flashcard**:  
   - Remove flashcards using their ID.  

4. **Show All Flashcards**:  
   - View all saved flashcards along with their details (`ID`, `Category`, `Question`, `Answer`, `Progress`).  

5. **Practice Flashcards**:  
   - Select a category and practice flashcards from it.  
   - Tracks progress for correct answers and displays the correct answer for wrong attempts.  

6. **Search Flashcards**:  
   - Find flashcards by keywords in their `Category`, `Question`, or `Answer`.  

7. **Show Categories**:  
   - Display all categories along with the number of flashcards in each.  

8. **Persistent Storage**:  
   - Flashcards are saved in a file (`Flashcards.data`) to retain data across sessions.  

## Technologies Used  
- **Programming Language**: C++  
- **Features**:  
  - File handling for persistent storage.  
  - Dynamic memory allocation.  
  - Case-insensitive search and category filtering.  
  - ANSI escape codes for terminal color formatting.  
