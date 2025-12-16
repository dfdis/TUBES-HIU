#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // For system()
#include "tree.h"

using namespace std;

// Helper function to safely read a line of text (for common name or link) or a single word (for taxonomic name)
string readInput(bool isLine = false) {
    string input;
    // Consume the newline character left by previous cin >>
    if (cin.peek() == '\n') cin.ignore(); 
    
    if (isLine) {
        // Use for common name or URL (can contain spaces)
        getline(cin, input);
    } else {
        // Use for taxonomic names (single word)
        cin >> input;
    }
    return input;
}

// Helper function to open the link on the system's default browser
void openWikipediaLink(const string& url) {
    if (url.empty()) {
        cout << "[INFO] No Wikipedia link available for this node.\n";
        return;
    }

    cout << "\n[INFO] Attempting to open Wikipedia link: " << url << "\n";
    
    #ifdef _WIN32
        // Windows command
        string command = "start " + url;
    #endif

    // Execute the command
    system(command.c_str());
}


int main() {
    Node* root = nullptr;
    int pilihan;

    // --- Example Species Data ---
    const vector<string> greatWhiteTax = {"Chondrichthyes", "Lamniformes", "Lamnidae", "Carcharodon", "carcharias"};
    const string greatWhiteCommonName = "Great White Shark";
    const string greatWhiteWiki = "https://en.wikipedia.org/wiki/Great_white_shark"; // <-- NEW LINK
    
    const vector<string> tigerSharkTax = {"Chondrichthyes", "Carcharhiniformes", "Carcharhinidae", "Galeocerdo", "cuvier"};
    const string tigerSharkCommonName = "Tiger Shark";
    const string tigerSharkWiki = "https://en.wikipedia.org/wiki/Tiger_shark"; // <-- NEW LINK

    // Automatically populate a few paths for demonstration (with the new wiki link)
    root = addSpeciesPath(root, greatWhiteTax, greatWhiteCommonName, greatWhiteWiki); // ADDED WIKI LINK
    root = addSpeciesPath(root, tigerSharkTax, tigerSharkCommonName, tigerSharkWiki); // ADDED WIKI LINK
    cout << "\n[INFO] Two example shark species have been pre-inserted.\n";


    do {
        cout << "\n===== SHARK TAXONOMY TREE (ADT) =====\n";
        cout << "1. Add New Species Path (C)\n";
        cout << "2. Search Taxonomic or Common Name (R)\n";
        cout << "3. Display Full Taxonomy Tree (R)\n";
        cout << "4. Update Species Details (U)\n";
        cout << "5. Delete Species (D)\n";
        cout << "6. Exit\n";
        cout << "Pilih menu: ";
        if (!(cin >> pilihan)) { 
            cout << "Input tidak valid. Bersihkan buffer.\n";
            cin.clear();
            readInput(true); 
            pilihan = 0;
            continue;
        }

        switch (pilihan) {
            case 1: {
                // --- ADD NEW SPECIES (CREATE) ---
                cout << "\n--- Add New Species ---\n";
                vector<string> taxonomicPath;
                string inputName;
                bool insertionFailed = false;

                // Loop through the 5 taxonomic levels
                for (const auto& level : TAX_LEVELS) {
                    cout << "Enter " << level << " name: ";
                    inputName = readInput();
                    if (inputName.empty()) {
                         cout << "[ERROR] Name cannot be empty. Insertion aborted.\n";
                         insertionFailed = true;
                         if (cin.fail()) cin.clear();
                         break;
                    }
                    taxonomicPath.push_back(inputName);
                }
                
                if (insertionFailed) break;
                
                // Get the Common Name
                cout << "Enter Common Name: ";
                string commonNameInput = readInput(true);
                if (commonNameInput.empty()) {
                    cout << "[ERROR] Common name cannot be empty. Insertion aborted.\n";
                    break;
                }
                
                // Get the Wikipedia Link
                cout << "Enter Wikipedia Link (URL, optional): ";
                string wikiLinkInput = readInput(true);
                
                // Call the updated function
                root = addSpeciesPath(root, taxonomicPath, commonNameInput, wikiLinkInput); 
                
                break;
            }
            case 2: {
                // --- SEARCH (READ) ---
                cout << "\n--- Search Name ---\n";
                cout << "Enter the name to search (Taxonomic name OR Common name): ";
                string searchName;
                if (cin.peek() == '\n') cin.ignore();
                getline(cin, searchName);
                
                if (searchName.empty()) {
                     cout << "[INFO] Search name cannot be empty.\n";
                     break;
                }

                Node* found = searchNode(root, searchName);
                if (found) {
                    cout << "\n[SUCCESS] Data '" << searchName << "' ditemukan.\n";
                    cout << "Level: " << found->level << "\n";
                    cout << "Taxonomic Name: " << found->name << "\n";
                    if (!found->commonName.empty()) {
                        cout << "Common Name: " << found->commonName << "\n";
                    }
                    if (!found->wikiLink.empty()) {
                        cout << "Wikipedia Link: " << found->wikiLink << "\n";
                        cout << "Want to open the link now? (y/n): ";
                        string openChoice = readInput();
                        if (toLower(openChoice) == "y") {
                            openWikipediaLink(found->wikiLink);
                        }
                    } else if (found->level == "Species") {
                         cout << "[INFO] No Wikipedia link recorded for this species.\n";
                    }
                    cout << "Children Count: " << found->children.size() << "\n";
                } else {
                    cout << "[INFO] Data '" << searchName << "' tidak ditemukan.\n";
                }
                break;
            }
            case 3: {
                // --- DISPLAY (READ ALL) ---
                cout << "\n--- Full Shark Taxonomy Tree ---\n";
                if (root) {
                    // 
                    displayTree(root);
                } else {
                    cout << "The tree is currently empty.\n";
                }
                break;
            }
            case 4: {
                // --- UPDATE SPECIES (U) ---
                cout << "\n--- Update Species Details ---\n";
                cout << "Enter the Taxonomic or Common Name of the SPECIES to update: ";
                string updateSearchName;
                if (cin.peek() == '\n') cin.ignore();
                getline(cin, updateSearchName);

                if (updateSearchName.empty()) {
                     cout << "[INFO] Name cannot be empty.\n";
                     break;
                }

                Node* speciesToUpdate = searchNode(root, updateSearchName);
                if (speciesToUpdate && speciesToUpdate->level == "Species") {
                    cout << "\n[FOUND] Species: " << speciesToUpdate->commonName << " (" << speciesToUpdate->name << ")\n";
                    
                    cout << "Enter NEW Common Name (Current: " << speciesToUpdate->commonName << "): ";
                    string newCommonName = readInput(true);
                    
                    cout << "Enter NEW Wikipedia Link (Current: " << speciesToUpdate->wikiLink << "): ";
                    string newWikiLink = readInput(true);
                    
                    if (newCommonName.empty()) {
                        cout << "[ERROR] Common Name cannot be empty. Update aborted.\n";
                        break;
                    }

                    updateSpecies(speciesToUpdate, newCommonName, newWikiLink);
                    
                } else if (speciesToUpdate && speciesToUpdate->level != "Species") {
                    cout << "[ERROR] Found '" << updateSearchName << "' but it is a " << speciesToUpdate->level << ". Only SPECIES can be updated.\n";
                } else {
                    cout << "[INFO] Species '" << updateSearchName << "' not found.\n";
                }
                break;
            }
            case 5: {
                // --- DELETE SPECIES (D) ---
                cout << "\n--- Delete Species ---\n";
                cout << "Enter the Taxonomic or Common Name of the SPECIES to delete: ";
                string deleteSearchName;
                if (cin.peek() == '\n') cin.ignore();
                getline(cin, deleteSearchName);
                
                if (deleteSearchName.empty()) {
                     cout << "[INFO] Name cannot be empty. Deletion aborted.\n";
                     break;
                }
                
                Node* found = searchNode(root, deleteSearchName);
                if (found && found->level == "Species") {
                    cout << "Are you sure you want to delete species '" << found->commonName << " (" << found->name << ")'? (y/n): ";
                    string confirm = readInput();
                    if (toLower(confirm) == "y") {
                        deleteSpecies(root, deleteSearchName); 
                    } else {
                        cout << "[INFO] Deletion cancelled.\n";
                    }
                } else if (found && found->level != "Species") {
                     cout << "[ERROR] Found '" << deleteSearchName << "' but it is a " << found->level << ". Only SPECIES can be deleted.\n";
                } else {
                    cout << "[INFO] Species '" << deleteSearchName << "' not found.\n";
                }
                break;
            }
            case 6:
                cout << "Keluar dari program. Membersihkan memori...\n";
                deleteTree(root); 
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            }
    } while (pilihan != 6);
    
    return 0;
}