#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> // Diperlukan untuk std::remove di tree.cpp, meskipun seringnya ditaruh di .cpp

//menjelaskan taxonomic yg fix untuk level strukturnya
const std::vector<std::string> TAX_LEVELS = {
    "Class", "Order", "Family", "Genus", "Species"
};
const size_t REQUIRED_TAX_LEVELS = TAX_LEVELS.size(); //ada 5 level taxonmic
const size_t REQUIRED_TOTAL_INPUTS = TAX_LEVELS.size() + 1; // 6 itu 5 level + 1 nama biasa(common name)

// sruktur nodenya
struct Node {
    std::string name;             //"Chondrichthyes", "carcharias" (taxonomic nya)
    std::string level;            //"Class", "Species" 
    std::string commonName;       //"Great White Shark" ini cuman buat spesies levelny
    std::string wikiLink;         // <-- NEW: Link Wikipedia
    std::vector<Node*> children;  // list pointer untuk node anakan
};

// --- FUNGSI UTILITY ---
std::string toLower(const std::string& str); // Deklarasi ulang agar bisa digunakan di mana saja
Node* createNode(const std::string& name, const std::string& level);

// --- FUNGSI CRUD: CREATE (Add) ---
// Update parameter untuk menerima wikiLink
Node* addSpeciesPath(Node* root, const std::vector<std::string>& path, const std::string& commonName, const std::string& wikiLink);

// --- FUNGSI CRUD: READ (Search/Display) ---
Node* searchNode(Node* root, const std::string& name);
void displayTree(Node* root, int depth = 0);

// --- FUNGSI CRUD: UPDATE ---
bool updateSpecies(Node* speciesNode, const std::string& newCommonName, const std::string& newWikiLink);

// --- FUNGSI CRUD: DELETE ---
// Fungsi ini juga mengembalikan Node* agar dapat menangani pembaruan root (meskipun dalam kasus ini tidak perlu karena Class tidak boleh dihapus)
// Parent digunakan untuk menghapus anak dari daftar anak Parent.
Node* deleteSpecies(Node* root, const std::string& speciesName, Node* parent = nullptr);

// Fungsi hapus seluruh tree (untuk exit)
void deleteTree(Node* root);

#endif