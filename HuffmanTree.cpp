#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <queue> 
#include <vector>
#include <fstream>
#include <iostream>
using std::stringstream;
using namespace std;
//Node class to declare program variables used by the functions to create and enforce the Huffman Tree algorithm. 
struct Node {
    Node* right;
    Node* left;
    char value;
    size_t charVal;
    Node(char value, size_t charVal = 0, Node* left = nullptr, Node* right = nullptr) {
        this->left = left;
        this->right = right;
        this->value = value;
        this->charVal = charVal;
    }
};
//Comparator function to pass values into priority queue. 
struct comparator {
    comparator() = default;
    bool operator()(Node* leftBranch, Node* rightBranch) {
        return leftBranch->charVal > rightBranch->charVal;
    }
};

class huffman_tree {
private:
    //Private methods that declare helper methods and unordered map data structure. 
    std::unordered_map<char, std::string> map;
    Node* root;

    void constructTree(const std::string& input);
    void nodePriorityQ(std::priority_queue<Node*, std::vector<Node*>, comparator>& q, const std::string& fileStr);
    void constructTreeVal(std::unordered_map<char, unsigned int>& treeVal, const std::string& fileStr);

    void constructNode(Node* node, std::string code);
    void readFile(const std::string& input, std::string& charVal) const;
    void traverseTree(Node*& node, char& charVal, char& bit, bool& child) const;

public:
    huffman_tree(const std::string& input);
    string get_character_code(char character) const;
    string encode(const std::string& input) const;
    string decode(const std::string& string_to_decode) const;
};
//Huffman constructor that takes string input that calls construct tree method to create tree and insert bit value of O into the root. 
huffman_tree::huffman_tree(const string& input) {
    root = nullptr;
    constructTree(input);
    if (root != nullptr) {
        if (root->left == nullptr && root->right == nullptr) {
            map.emplace(root->value, "0");
        }
        else {
            constructNode(root, "");
        }
    }
}

//Get character method that takes character from string and returns the Huffman code for character if character is in the tree and or an empty string. 
string huffman_tree::get_character_code(char character) const {
    auto iter = map.find(character);
    if (iter != map.end()) {
        return iter->second;
    }
    else {
        return "";
    }
}
//Encode function that takes string input and encodes the contents of input. If the input contains letters that aren't in the huffman_tree, it returns an empty string
string huffman_tree::encode(const string& input) const {
    string content = "";
    string fileContents = "";
    readFile(input, fileContents);
    if (fileContents.empty()) {
        return content;
    }
    for (char c : fileContents) {
        auto iter = map.find(c);
        if (iter == map.end()) {
            return "";
        }
        else {
            content += iter->second;
        }
    }
    return content;
}
//Decode function that takes in a string input and returns the regular string text if the string is a valid Huffman encoding. If it isn't, then it returns an empty string. 
string huffman_tree::decode(const string& string_to_decode) const {
    char comparison;
    string content = "";
    if (root == nullptr) {
        return content;
    }
    else if (root->left == nullptr && root->right == nullptr) {
        comparison = map.find(root->value)->second[0];
        for (char bit : string_to_decode) {
            if (bit != comparison) {
                return "";
            }
            else {
                content += root->value;
            }
        }
        return content;
    }
    Node* node = root;
    char letter;
    bool leafNode;
    //Iteration over string to decode each character. Calls traverseTree method to iterate over tree and compare whether character matches an encode character. 
    for (char c : string_to_decode) {
        leafNode = false;
        comparison = c;
        traverseTree(node, letter, c, leafNode);
        if (c != comparison) {
            return "";
        }
        else if (leafNode) {
            content += letter;
            node = root;
        }
    }
    if (leafNode) {
        return content;
    }
    else {
        return "";
    }
}
//Construct tree function that creates the tree using string input. Utilizes priority queue to create and insert nodes into tree. 
void huffman_tree::constructTree(const std::string& input) {
    comparator comp;
    string inputString;
    priority_queue<Node*, vector<Node*>, comparator> q;
    readFile(input, inputString);
    nodePriorityQ(q, inputString);
    //As long as the size of the queue is greater than one, queue pops of the characters with the lowest frequency and puts it into tree. 
    while (q.size() > 1) {
        Node* first = q.top();
        q.pop();
        Node* next = q.top();
        q.pop();
        Node* newNode = new Node('~');
        if (comp(first, next)) {
            newNode->left = next;
            newNode->right = first;
        }
        else {
            newNode->left = first;
            newNode->right = next;
        }
        newNode->charVal = first->charVal + next->charVal;
        q.push(newNode);
    }
    root = q.top();
}
//Priority Queue function that inserts tree nodes into a map that iterates over each element to find a matching node that can be inserted into the main queue.
void huffman_tree::nodePriorityQ(priority_queue<Node*, vector<Node*>, comparator>& q, const string& charFile) {
    unordered_map<char, unsigned int> treeVal;
    constructTreeVal(treeVal, charFile);
    if (treeVal.empty()) {
        q.emplace(nullptr);
        return;
    }
    for (pair<char, unsigned int> p : treeVal) {
        Node* node = new Node(p.first, p.second);
        q.emplace(node);
    }
}
//Constructing node function that builds each node with the desired character and puts it into a map that is then inserted into queue. 
void huffman_tree::constructTreeVal(unordered_map<char, unsigned int>& treeVal, const std::string& charFile) {
    unordered_map<char, unsigned int>::iterator iter;
    for (char c : charFile) {
        iter = treeVal.find(c);
        if (iter == treeVal.end()) {
            treeVal.emplace(c, 1);
        }
        else {
            ++(iter->second);
        }
    }
}

//Construct node function that puts each node with the bit vlaue into the tree by traversing each node and determining whether it is the right place. 
void huffman_tree::constructNode(Node* node, string val) {
    if (node->left == nullptr && node->right == nullptr) {
        map.emplace(node->value, val);
    }
    else if (node->right == nullptr) {
        constructNode(node->left, val + '0');
    }
    else if (node->left == nullptr) {
        constructNode(node->right, val + "1");
    }
    else {
        constructNode(node->left, val + '0');
        constructNode(node->right, val + '1');
    }
}
//Read file method that takes in string input of file name and reads the contents of the file to send to other methods for a file input. 
void huffman_tree::readFile(const string& input, string& charVal) const {
    if (input[0] != '/') {
        charVal = input;
        return;
    }
    string file(input.cbegin() + 1, input.cend());
    ifstream streamFile;
    streamFile.open(file);
    stringstream read;
    bool ch = true;
    if (streamFile.is_open()) {
        while (read << streamFile.rdbuf()) {
            ch = false;
        }
    }
    charVal = std::move(read.str());
}
//Traverse tree method that traverses tree and sets a node's bit value based on whether it's on the right or left side. 
void huffman_tree::traverseTree(Node*& node, char& charVal, char& bitVal, bool& leafNode) const {
    if (bitVal == '1') {
        node = node->right;
    }
    else {
        node = node->left;
    }
    if (node == nullptr) {
        ++bitVal;
    }
    else {
        charVal = node->value;
        leafNode = node->left == nullptr && node->right == nullptr;
    }
}









