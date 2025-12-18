#include "tree.h"
#include <algorithm> // For std::transform, std::remove
#include <cctype>    // For ::tolower
#include <iostream>
#include <queue>     // Diperlukan untuk Level Order Traversal

/**
 * @brief Helper function to convert a string to lowercase for case-insensitive comparison.
 * DIDEFINISIKAN HANYA DI SINI.
 */
std::string toLower(const std::string& str) {
    std::string data = str;
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return data;
}

/**
 * @brief Creates a new tree node.
 */
Node* createNode(const std::string& name, const std::string& level) {
    Node* newNode = new Node();
    newNode->name = name;
    newNode->level = level;
    newNode->commonName = "";   
    newNode->wikiLink = "";     
    return newNode;
}

/**
 * @brief Recursively finds a child node with a specific name.
 */
Node* findChild(Node* parent, const std::string& name) {
    std::string lowerName = toLower(name);
    for (Node* child : parent->children) {
        if (toLower(child->name) == lowerName) {
            return child;
        }
    }
    return nullptr;
}

/**
 * @brief Inserts a full taxonomic path (Class down to Species) into the tree.
 */
Node* addSpeciesPath(Node* root, const std::vector<std::string>& path, const std::string& commonName, const std::string& wikiLink) {
    if (path.size() != REQUIRED_TAX_LEVELS) {
        std::cerr << "Internal Error: Path size mismatch in addSpeciesPath.\n";
        return root;
    }

    Node* currentNode = root;
    std::string newClassNameLower = toLower(path[0]);
    
    for (size_t i = 0; i < path.size(); ++i) {
        const std::string& name = path[i];
        std::string level = TAX_LEVELS[i];

        if (i == 0) {
            if (currentNode == nullptr) {
                currentNode = createNode(name, level);
                root = currentNode;
            } else if (toLower(currentNode->name) != newClassNameLower) {
                std::cerr << "Error: The tree already has a Class: " << currentNode->name 
                          << ". All shark species must belong to the same Class.\n";
                return root;
            }
        } else {
            Node* existingChild = findChild(currentNode, name);
            
            if (existingChild) {
                currentNode = existingChild;
                
                if (i == path.size() - 1) {
                     // Update existing species details
                     if (existingChild->commonName != commonName || existingChild->wikiLink != wikiLink) {
                         existingChild->commonName = commonName;
                         existingChild->wikiLink = wikiLink;
                         std::cout << "[INFO] Species '" << name << "' already exists. Updating its details.\n";
                     }
                }
            } else {
                Node* newNode = createNode(name, level);
                currentNode->children.push_back(newNode);
                
                if (i == path.size() - 1) {
                    newNode->commonName = commonName;
                    newNode->wikiLink = wikiLink; 
                    std::cout << "--> Successfully added new species: " << commonName << " (" << name << ")\n";
                } else {
                    std::cout << "--> Inserting new " << level << ": " << name << "\n";
                }
                
                currentNode = newNode;
            }
        }
    }
    
    return root;
}


/**
 * @brief Recursively searches the tree for a node with a matching taxonomic OR common name.
 */
Node* searchNode(Node* root, const std::string& name) {
    if (root == nullptr) {
        return nullptr;
    }
    std::string lowerName = toLower(name);

    if (toLower(root->name) == lowerName) {
        return root;
    }
    
    if (!root->commonName.empty() && toLower(root->commonName) == lowerName) {
        return root;
    }

    for (Node* child : root->children) {
        Node* result = searchNode(child, name);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

// --- CRUD: UPDATE IMPLEMENTATION ---
/**
 * @brief Updates the common name and Wikipedia link of a specific Species node.
 */
bool updateSpecies(Node* speciesNode, const std::string& newCommonName, const std::string& newWikiLink) {
    if (speciesNode == nullptr || speciesNode->level != "Species") {
        std::cerr << "[ERROR] Cannot update node details. Must be a valid Species node.\n";
        return false;
    }

    speciesNode->commonName = newCommonName;
    speciesNode->wikiLink = newWikiLink;
    
    std::cout << "[SUCCESS] Species '" << speciesNode->name << "' updated.\n";
    
    return true;
}

// --- CRUD: DELETE IMPLEMENTATION ---
/**
 * @brief Recursively searches for and deletes a specific Species node by its name.
 */
Node* deleteSpecies(Node* root, const std::string& speciesName, Node* parent) {
    if (root == nullptr) {
        return nullptr;
    }

    std::string lowerSpeciesName = toLower(speciesName);
    
    if (root->level == "Species" && 
        (toLower(root->name) == lowerSpeciesName || toLower(root->commonName) == lowerSpeciesName)) {
        
        if (parent == nullptr) {
            std::cerr << "[ERROR] Cannot delete the absolute root node (" << root->name << ").\n";
            return root;
        }

        // Remove the pointer from the parent's children vector
        auto& children = parent->children;
        auto it = std::remove(children.begin(), children.end(), root);
        children.erase(it, children.end());
        
        std::cout << "[SUCCESS] Species '" << root->commonName << " (" << root->name << ")' deleted.\n";
        delete root;
        
        return parent; 
    }

    // Pass the current root as the parent for the next level
    for (Node* child : root->children) {
        deleteSpecies(child, speciesName, root); 
    }
    
    return root; 
}


/**
 * @brief Displays the tree structure using indentation.
 */
void displayTree(Node* root, int depth) {
    if (!root) return;

    for (int i = 0; i < depth; ++i) {
        std::cout << (i == depth - 1 ? "  |--" : "  |  ");
    }

    std::cout << "(" << root->level << ") " << root->name;
    
    if (root->level == "Species") {
        if (!root->commonName.empty()) {
            std::cout << " [" << root->commonName << "]";
        }
        if (!root->wikiLink.empty()) { 
            std::cout << " {W}"; 
        }
    }
    std::cout << "\n";

    for (Node* child : root->children) {
        displayTree(child, depth + 1);
    }
}

/**
 * @brief Cleans up all dynamically allocated memory in the tree.
 */
void deleteTree(Node* root) {
    if (!root) return;
    for (Node* child : root->children) {
        deleteTree(child);
    }
    delete root;
}

// --- TRAVERSAL IMPLEMENTATIONS ---

void preOrderTraversal(Node* root) {
    if (!root) return;

    std::cout << root->level << ": " << root->name;
    if (!root->commonName.empty()) {
        std::cout << " [" << root->commonName << "]";
    }
    std::cout << "\n";

    for (Node* child : root->children) {
        preOrderTraversal(child);
    }
}

void postOrderTraversal(Node* root) {
    if (!root) return;

    for (Node* child : root->children) {
        postOrderTraversal(child);
    }

    std::cout << root->level << ": " << root->name;
    if (!root->commonName.empty()) {
        std::cout << " [" << root->commonName << "]";
    }
    std::cout << "\n";
}

void levelOrderTraversal(Node* root) {
    if (!root) return;

    std::queue<Node*> q; 
    q.push(root);

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();

        std::cout << current->level << ": " << current->name;
        if (!current->commonName.empty()) {
            std::cout << " [" << current->commonName << "]";
        }
        std::cout << "\n";

        for (Node* child : current->children) {
            q.push(child);
        }
    }
}
