#include "tree.h"
#include <algorithm> // For std::transform
#include <cctype>    // For ::tolower

/**
 * @brief Helper function to convert a string to lowercase for case-insensitive comparison.
 */
std::string toLower(const std::string& str) {
    std::string data = str;
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return data;
}

/**
 * @brief Creates a new tree node.
 * * @param name The taxonomic name (e.g., "Carcharodon").
 * @param level The taxonomic level (e.g., "Genus").
 * @return Node* A pointer to the newly created node.
 */
Node* createNode(const std::string& name, const std::string& level) {
    Node* newNode = new Node();
    newNode->name = name;
    newNode->level = level;
    newNode->commonName = ""; // Initialize common name as empty
    // children vector is initialized as empty
    return newNode;
}

/**
 * @brief Recursively finds a child node with a specific name.
 * * @param parent The current node to search its children.
 * @param name The name of the child to find.
 * @return Node* The pointer to the found child, or nullptr if not found.
 */
Node* findChild(Node* parent, const std::string& name) {
    // Using case-insensitive comparison for finding existing nodes
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
 * * @param root The root of the overall tree or the current node for recursion.
 * @param path A vector of strings representing the 5 taxonomic names (Class to Species).
 * @param commonName The common name of the species to be stored on the Species node.
 * @return Node* The (possibly new) root of the tree.
 */
Node* addSpeciesPath(Node* root, const std::vector<std::string>& path, const std::string& commonName) {
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
            // Handling the absolute root (Class level)
            if (currentNode == nullptr) {
                currentNode = createNode(name, level);
                root = currentNode;
            } else if (toLower(currentNode->name) != newClassNameLower) {
                std::cerr << "Error: The tree already has a Class: " << currentNode->name 
                          << ". All shark species must belong to the same Class.\n";
                return root;
            }
        } else {
            // Handling nodes from Order down to Species
            Node* existingChild = findChild(currentNode, name);
            
            if (existingChild) {
                currentNode = existingChild;
                
                // If the Species node already exists, update its common name if necessary
                if (i == path.size() - 1) {
                     // Only update and log if the name is actually different
                     if (existingChild->commonName != commonName) {
                         std::cout << "[INFO] Species '" << name << "' already exists. Updating common name to '" << commonName << "'.\n";
                         existingChild->commonName = commonName;
                     } else {
                         std::cout << "[INFO] Species '" << name << "' already exists with the same common name.\n";
                     }
                }
            } else {
                // Node does not exist, create it and link it
                Node* newNode = createNode(name, level);
                currentNode->children.push_back(newNode);
                
                // If this is the final Species node, set the common name
                if (i == path.size() - 1) {
                    newNode->commonName = commonName;
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
 * * @param root The current node to start the search from.
 * @param name The name to search for.
 * @return Node* The pointer to the found node, or nullptr if not found.
 */
Node* searchNode(Node* root, const std::string& name) {
    if (root == nullptr) {
        return nullptr;
    }

    // Use case-insensitive comparison for search
    std::string lowerName = toLower(name);

    // 1. Check current node (Taxonomic name)
    if (toLower(root->name) == lowerName) {
        return root;
    }
    
    // 2. Check current node (Common name - only present on Species nodes)
    if (!root->commonName.empty() && toLower(root->commonName) == lowerName) {
        return root;
    }

    // Recursively check children
    for (Node* child : root->children) {
        Node* result = searchNode(child, name);
        if (result != nullptr) {
            return result; // Found the node in a subtree
        }
    }

    return nullptr; // Not found in this branch
}

/**
 * @brief Displays the tree structure using a simple indentation for hierarchy.
 * * @param root The current node to display.
 * @param depth The current depth (for indentation).
 */
void displayTree(Node* root, int depth) {
    if (!root) return;

    // Print indentation
    for (int i = 0; i < depth; ++i) {
        std::cout << (i == depth - 1 ? "  |--" : "  |  ");
    }

    // Print the node information
    std::cout << "(" << root->level << ") " << root->name;
    
    // If it's a species node and has a common name, display it
    if (root->level == "Species" && !root->commonName.empty()) {
        std::cout << " [" << root->commonName << "]";
    }
    std::cout << "\n";

    // Recursively call for all children
    for (Node* child : root->children) {
        displayTree(child, depth + 1);
    }
}

/**
 * @brief Cleans up all dynamically allocated memory in the tree.
 * * @param root The root of the tree to be deleted.
 */
void deleteTree(Node* root) {
    if (!root) return;
    for (Node* child : root->children) {
         deleteTree(child);
    }
    delete root;
}