/*Luca Santarella 22/06/23

HUFFMAN CODING (SEQUENTIAL):

*/
// C++ program for Huffman Coding
#include <queue>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
#include "utimer.hpp"
#include <chrono>
#include <bitset>
using namespace std;

#define MAX_TREE_HT 1000
long n_chars;
int printFlag = 0;

//struct representin tree node
struct treeNode
{
    char data;

    int freq;

    //left and right children
    struct treeNode *left, *right;
};

//struct representing whole Huffman tree
struct tree
{
    int size;

    struct treeNode *root;

};

struct node_comparison 
{
   bool operator()( const treeNode* a, const treeNode* b ) const 
   {
    return a->freq > b->freq;
   }
};

struct treeNode* newNode(char data, int freq)
{
    struct treeNode* myNewNode = (struct treeNode*) malloc(sizeof(struct treeNode));
    myNewNode->left = nullptr;
    myNewNode->right = nullptr;
    myNewNode->data = data;
    myNewNode->freq = freq;

    return myNewNode;
}

std::map<char, int> countFreq(std::string str)
{
    long usecs;
    // size of the string 'str'
    int n = str.size();

    //map used to store the couple <str, #occ>
    std::map<char, int> mapStrOcc;

    {utimer t0("counting freq", &usecs);

        // accumulate frequency of each character in 'str'
        for (int i = 0; i < n; i++)
            mapStrOcc[str[i]]++;
    }
    if(printFlag)
        std::cout << "counting freq in " << usecs << " usecs" << std::endl;
    return mapStrOcc;
}


void printFreq(std::map<char, int> map)
{
    // Get an iterator pointing to the first element in the map
    std::map<char, int>::iterator it = map.begin();
    while (it != map.end())
    {
        std::cout << "key: " << it->first << ", freq: " << it->second << std::endl;
        ++it;
    }
}

void printMap(std::map<char, std::string> map)
{
    // Get an iterator pointing to the first element in the map
    std::map<char, std::string>::iterator it = map.begin();
    while (it != map.end())
    {
        std::cout << "key: " << it->first << ", code: " << it->second << std::endl;
        ++it;
    }
}

template<typename Q>
void initQueue(Q &prior_q, std::map<char, int> freq, tree* &hufTree)
{
    long usecs;
    {utimer t0("init q", &usecs);
        std::map<char, int>::iterator it = freq.begin();
        while (it != freq.end())
        {
            struct treeNode *myNewNode;
            myNewNode = newNode(it->first, it->second);
            prior_q.push(myNewNode);
            hufTree->size++;
            ++it;
        }
    }
    if(printFlag)
        std::cout << "initialize queue in " << usecs << " usecs" << endl;
}

template<typename Q>
void printQueue(std::string_view name, Q q)
{
    for (std::cout << name << ": \n"; !q.empty(); q.pop())
        std::cout << "key: " << q.top()->data << " freq: " << q.top()->freq << std::endl ;
    std::cout << '\n';
}

// function to check if this node is leaf
int isLeaf(struct treeNode* node)
{

    return !(node->left) && !(node->right);
}

//print an array of size n
void printArr(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        std::cout << arr[i];

    std::cout << "\n";
}

void setCode(char data, int arr[], int n, std::map<char, std::string> &codes)
{
    std::string code;
    for (int i=0; i < n; i++)
    {
        code += to_string(arr[i]);
    }
    codes[data] = code;
}


void traverseTree(struct treeNode* root, int arr[], int top, std::map<char, std::string> &codes)
{
    //assign 0 to left edge and recur
    if (root->left) {

        arr[top] = 0;
        traverseTree(root->left, arr, top + 1, codes);
    }

    //assign 1 to right edge and recur
    if (root->right) {

        arr[top] = 1;
        traverseTree(root->right, arr, top + 1, codes);
    }

    if (isLeaf(root)) {

        setCode(root->data, arr, top, codes);
    }
}

void freeTree(struct treeNode* &root)
{
    if(root == nullptr)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}

template<typename Q>
void buildHufTree(Q &prior_q, tree* &hufTree)
{
    long usecs;
    {utimer t0("build huf tree", &usecs);
        while(prior_q.size() != 1)
        {
            //take first node with the lowest freq
            struct treeNode *firstNode = prior_q.top();

            //remove it from the priority queue
            prior_q.pop();

            //take second node and do the same
            struct treeNode *secondNode = prior_q.top();
            prior_q.pop();

            //compute the sum between the two nodes
            int sum = firstNode->freq + secondNode->freq;

            //create new internal node 
            // $ special character to denote internal nodes with no char
            struct treeNode *internalNode = newNode('$', sum);

            //set children of new internal node
            internalNode->left = firstNode;
            internalNode->right = secondNode;

            //push internal node to priority queue
            prior_q.push(internalNode);

            //increase size of binary tree because of new internal node
            hufTree->size++;
        }
    }

    if(printFlag)
        cout << "building Huffman tree in " << usecs << " usecs" << endl;
}

std::string HuffmanCoding(std::string stringToCode, std::map<char, std::string> codes)
{
    std::string codedStr;

    int n = stringToCode.size();
    long usecs;

    {utimer t0("huffman coding", &usecs);
        for(int i=0; i <n; i++)
        {
            char charToCode = stringToCode[i];
            codedStr += codes[charToCode];
        }
    }

    if(printFlag)
        cout << "huffman coding in " << usecs << " usecs" << endl;
    return codedStr;
}

std::string padCodedStr(std::string str)
{
    int size = str.size();
    int bits = size % 8;
    bits = 8 - bits;

    //pad the string
    str.append(bits, '0');
    return str;
}

char convertToASCII(std::string binaryString)
{
    int decimalValue = 0;
    for (char bit : binaryString) 
        decimalValue = (decimalValue << 1) + (bit - '0');

    return static_cast<char>(decimalValue);
}

std::string encodeStrASCII(std::string binaryString)
{
    std::string encodedStr;
    for(int i=0; i<binaryString.size(); i+=8)
        encodedStr += convertToASCII(binaryString.substr(i, 8));

    return encodedStr;
}

int main(int argc, char* argv[])
{
    if(argc == 2 && strcmp(argv[1],"-help")==0) {
        std::cout << "Usage:\n" << argv[0] << " -v" << std::endl;
        return(0);
    }
    
    if(argc > 1 && strcmp(argv[1],"-v") == 0)
        printFlag = 1;    // flag for printing

    //***READING FROM TXT FILE***
    std::string strFile;
    std::string str;
    long usecs;
    std::string inputFilename = "file_10M";
    {utimer t0("reading file", &usecs);
        
        ifstream inFile("txt_files/"+inputFilename+".txt");
        if (!inFile.is_open()) 
        {
            std::cout << "Failed to open the file." << std::endl;
            return 1;
        }            
        while(getline (inFile, str))
        {
            strFile += str;
            //strFile.push_back('\n');
        }

        inFile.close();
    }
    if(printFlag)
        std::cout << "reading in " << usecs << " usecs" << std::endl;

    //***COUNTING FREQUENCIES***
    std::map<char, int> freq;
    freq = countFreq(strFile);
    //if(printFlag)
    //    printFreq(freq);
    n_chars = freq.size();

    //***INITIALIZE PRIORITY QUEUE AND BINARY TREE***
    // Max priority to lowest freq node
    std::priority_queue<treeNode*, vector<treeNode*>, node_comparison> prior_q; 

    //representation of the binary tree
    struct tree *hufTree = (struct tree*) malloc (sizeof(struct tree));
    hufTree->size = 0;

    //initialize the priority queue
    initQueue(prior_q, freq, hufTree);

    //*** BUILD HUFFMAN TREE
    //build the huffman tree using the priority queue
    buildHufTree(prior_q, hufTree);

    //set root 
    struct treeNode* myRoot = prior_q.top();
    hufTree->root = myRoot;

    //array used to get Huffman codes
    int arr[MAX_TREE_HT], top = 0;

    //map <char, huffman code>
    std::map<char, std::string> codes;

    //*GET HUFFMAN CODES USING HUFFMAN TREE
    //traverse the Huffman tree and set codes
    traverseTree(myRoot, arr, top, codes);

    //if(printFlag)
        //printMap(codes);

    //*** HUFFMAN CODING ***
    std::string codedStr = HuffmanCoding(strFile, codes);

    //pad the coded string to get a multiple of 8
    if(codedStr.size() % 8 != 0)
        codedStr = padCodedStr(codedStr);

    //encode binary string (result of Huffman coding) as ASCII characters 
    codedStr = encodeStrASCII(codedStr);

    //*** WRITING TO FILE ***
    {utimer t0("writing file", &usecs);
        std::ofstream outFile("out_files/coded_"+inputFilename+".txt", std::ios::binary);

        if (outFile.is_open()) 
        {
            outFile.write(codedStr.c_str(), codedStr.size());
            outFile.close();  // Close the file
        }
        else
        {
            std::cout << "Unable to open the file." << std::endl;
        }
    }
    if(printFlag)
        std::cout << "writing in " << usecs << " usecs" << std::endl;

    //*** FREE MEMORY ***
    freeTree(myRoot);
    free(hufTree);

    return (0);
}
