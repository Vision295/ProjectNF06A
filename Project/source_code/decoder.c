/**
 * @file decoder.c
 * @author Lucas SCHUMMER
 * @brief The file decoder that uses the Huffman Coding Algorithm
 * @version 1.0
 * @date 28/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief   An element of the Huffman Tree
 *          weight is the number of occurencies in the file of the byte represented by the Node
 *          byte is the byte stored by the Node (in case the Node is a "leaf" of the tree)
 *          left and right point to other Nodes, the one under it in the tree (only if Node is not a "leaf")
 */
struct Node{
    int weight;
    unsigned char byte;
    struct Node *right;
    struct Node *left;
};


/**
 * @brief           Creates a struct Node from all its parameters
 * 
 * @param byte      The byte represented by the Node (only if it's a "leaf" of the tree, otherwise 0 (we will never use it in this case anyway))
 * @param weight    If the Node is a leaf, the weight is the number of occurencies in the file of the byte represented by the byte. 
 *                  Otherwise, it is the sum of the weights of the Nodes under it.
 * @param left      Points to another Node, which is lower in the tree. (NULL if the Node is a "leaf")
 * @param right     Points to another Node, which is lower in the tree. (NULL if the Node is a "leaf")
 * 
 * @return          The struct Node created
 */
struct Node Create_Node (unsigned char byte, int weight, struct Node* left, struct Node* right) {
    
    struct Node node;
    node.weight = weight;
    node.byte = byte;
    node.right = right;
    node.left = left;
    return node;

}


char* Read_Encoded_File_Name() {

    FILE* filename_file = fopen("../compressed/compressed.txt", "r");
    char* filename;
    fscanf(filename_file, "%s", filename);
    fclose(filename_file);
    return filename;

}


/**
 * @brief               Sorts the Node_List by ascending order of frequencies, knowing that the list is already sorted, except for the last element
 * 
 * @param Node_List     Array of struct Nodes, that we want to sort by frequencies
 * @param n             The length of the array Nodes_List
 */
void Insertion_Sort(struct Node* Node_List, int n) {

    int i=0;
    while (i<n && Node_List[n-1].weight >= Node_List[i].weight) {
        i++;
    }
    int index_to_insert = i;

    //Shifting every element which is after the insertion index one step to the right
    struct Node Node_Memory = Node_List[n-1];
    for (int j=n-1; j>index_to_insert; j--) {
        Node_List[j] = Node_List[j-1];
    }

    //Adding the last element at the right index
    Node_List[index_to_insert] = Node_Memory;

}


/**
 * @brief               From the sorted list of Nodes, create the Huffman Tree, according to the Huffman Coding Algorithm
 * 
 * @param n_diff_bytes  The number of different bytes (the length of the diff_bytes array)
 * @param Node_list     The array of struct Nodes, sorted by frequencies
 * @param memory_nodes  An array of struct Nodes, used to store the Nodes we remove from the Nodes_List (according to the Huffman Coding Algorithm)
 */
void Create_Huffman_Tree(int n_diff_bytes, struct Node* Node_list, struct Node* memory_nodes) {

    //Repeat the following instructions until we only have one Node left, the "head" of the tree
    int n_memory_nodes = 0;
    while (n_diff_bytes > 1) {

        //Copying the first 2 nodes into the memory list
        struct Node Memory_Node1;
        Memory_Node1.left = Node_list[0].left;
        Memory_Node1.right = Node_list[0].right;
        Memory_Node1.weight = Node_list[0].weight;
        Memory_Node1.byte = Node_list[0].byte;
        struct Node Memory_Node2;
        Memory_Node2.left = Node_list[1].left;
        Memory_Node2.right = Node_list[1].right;
        Memory_Node2.weight = Node_list[1].weight;
        Memory_Node2.byte = Node_list[1].byte;

        memory_nodes[n_memory_nodes] = Memory_Node1;
        n_memory_nodes++;
        memory_nodes[n_memory_nodes] = Memory_Node2;
        n_memory_nodes++;


        //Create a new node linking the two first ones in the list (lowest frequencies)
        struct Node Sum_Node = Create_Node((unsigned char) 0, memory_nodes[n_memory_nodes-2].weight + memory_nodes[n_memory_nodes-1].weight, &memory_nodes[n_memory_nodes-2], &memory_nodes[n_memory_nodes-1]);
        n_diff_bytes = n_diff_bytes - 2;


        //Erasing the two nodes by shifting the Nodes 2 indexes to the left
        for (int i=0; i<n_diff_bytes; i++) {
            Node_list[i] = Node_list[i+2];
        }


        //Adding the New Node to the list and sorting the list
        Node_list[n_diff_bytes] = Sum_Node;
        n_diff_bytes ++;
        Insertion_Sort(Node_list, n_diff_bytes);

    }

}


/**
 * @brief               Read the size of the encoded file 
 * 
 * @param encoded_file  The encoded binary file, already opened in read mode
 * 
 * @return              The size of the file (in bytes)
 */
int Read_Size(FILE* encoded_file) {

    //Get the byte length of the file
    fseek(encoded_file, 0, SEEK_END);
    int size_of_file = ftell(encoded_file);
    fseek(encoded_file, 0, SEEK_SET);

    return size_of_file;

}


/**
 * @brief                   Read the number of padding zeroes added by the encoder to the penultimate byte
 *                          For more details about padding zeroes, see encoder.c
 * 
 * @param encoded_file      The encoded binary file, opened in read mode
 * @return                  The number of padding zeroes (between 0 and 7) added to the penultimate byte 
 */
unsigned char Read_Padding_Zeroes(FILE* encoded_file) {

    //Get number of padding zeroes in the penultimate byte (which is written in the last byte)
    fseek(encoded_file, -1, SEEK_END);
    unsigned char padding_zeroes;
    fread(&padding_zeroes, sizeof(unsigned char), 1, encoded_file);
    
    //Go back to the start of the file
    fseek(encoded_file, 0, SEEK_SET);

    return padding_zeroes;
}


/**
 * @brief                   From the Huffman Tree that we've rebuilt and all the bytes of the encoded file, decode it by going through the Tree
 * 
 * @param size_of_file      The size (in bytes) of the encoded file
 * @param padding_zeroes    The number of padding zeroes in the penultimate byte
 * @param bytes             The bytes of the compressed file
 * @param Node_list         The list of Nodes, containing only one last element at this point, which is the head of the tree
 */
void Decode_Encoded_File(int size_of_file, unsigned char padding_zeroes, unsigned char* bytes, struct Node* Node_list) {
    
    //Read the name of the file that is currently encoded and open it in writing mode
    char* filename = Read_Encoded_File_Name();
    char path[] = "../compressed/";
    strcat(path, filename);
    FILE* decoded_file = fopen(path, "wb");
    if (decoded_file == NULL) printf("Failed to load file");


    //Index through the array bytes
    int n_bytes = 0;
    //How many bits of the actual byte we have taken off
    int n_bytes_i = 0;
    //Start at the head of the tree
    struct Node Node_Visiting = Node_list[0];


    //We read the bytes from the first one to the penultimate
    while (n_bytes < size_of_file-1) {

        //We've just reached the penultimate byte (the last one is the number of padding zeroes)
        if (n_bytes == size_of_file-2 && n_bytes_i == 0){
            //Make sure we won't read the padding zeroes at the end of the byte
            //Doing this, we will read 8-padding_zeroes bits of this byte
            n_bytes_i = padding_zeroes;
        }

        //If we hit a 1, go down the tree to the right, if it's a 0, go left
        if (bytes[n_bytes] >=128) {
            Node_Visiting = *Node_Visiting.right;
        } else {
            Node_Visiting = *Node_Visiting.left;
        }

        //Shift to the left (erase the first bit)
        bytes[n_bytes] = bytes[n_bytes] <<1;
        n_bytes_i++;

        //The byte has been fully read, we go to the next one
        if (n_bytes_i == 8) {
            n_bytes_i = 0;
            n_bytes++;
        }

        //We've reached a leaf of the tree
        if (Node_Visiting.left == NULL && Node_Visiting.right == NULL) {
            //Write the byte to the decoded file
            putc(Node_Visiting.byte, decoded_file);
            //Go back to the head of the tree
            Node_Visiting = Node_list[0];
        }

    }

    fclose(decoded_file);

}


/**
 * @brief               Read and store the bytes of the encoded file
 * 
 * @param encoded_file  The encoded file, opened in binary read mode
 * @param size_of_file  The size of the encoded file (in bytes)
 * @param bytes         The array of bytes where we store the bytes of the file that we read
 */
void Read_Encoded_Bytes(FILE* encoded_file, int size_of_file, unsigned char* bytes) {

    fread(bytes, sizeof(unsigned char), size_of_file, encoded_file);

}


/**
 * @brief   Read the conversion file to rebuild the Huffman Tree, read the encoded file and decoded it thanks to the Huffman Tree, and then
 *          writes the decoded file into a new file      
 * 
 * @return  0 if there were no erros in the process
 */
int main()
{

    //File to read from, so that we can rebuild the Huffman Tree
    FILE *conversion_file = fopen("../compressed/conversion.bin", "rb");

    //Create the structures to read the file
    int n_diff_bytes;
    unsigned char diff_bytes[256];

    //Read the number of different bytes(to rebuild the tree)
    n_diff_bytes = getw(conversion_file);
    fread(diff_bytes, sizeof(unsigned char), 256, conversion_file);

    //We have to allocate the memory with respect to n_diff_bytes
    int *frequencies;
    frequencies = (int*) calloc(n_diff_bytes, sizeof(int));

    fread(frequencies, sizeof(int), n_diff_bytes, conversion_file);


    fclose(conversion_file);


    //In this part, we will recreate the tree, the same way as in the encoder
    //Of course, it well be the exact same one, as we give it the exact same inputs


    //Creates the list of Nodes to add into the tree
    struct Node* Node_list = (struct Node*) calloc(n_diff_bytes, sizeof(struct Node));
    for (int i=0; i<n_diff_bytes; i++) {
        Node_list[i] = Create_Node(diff_bytes[i], frequencies[i], NULL, NULL);
    }


    //Creating a memory list of copy_nodes to store the ones we erase from Node_List, to keep track of them 
    //The tree can contain up to 512 nodes (the leaves + all the ones built from the sum of the two under)
    struct Node* memory_nodes = (struct Node*) calloc(512, sizeof(struct Node));
    int n_memory_nodes = 0;


    Create_Huffman_Tree(n_diff_bytes, Node_list, memory_nodes);


    //Open the encoded file and gather the information necessary to decode it
    FILE *encoded_file = fopen("../compressed/encoded.bin", "rb");
    int size_of_file = Read_Size(encoded_file);
    unsigned char padding_zeroes = Read_Padding_Zeroes(encoded_file);


    //Allocate and read the encoded bytes
    unsigned char *bytes;
    bytes = (unsigned char*) calloc(size_of_file, 1);
    if (bytes == NULL) {
        printf("\n bytes failed to load");
        exit(EXIT_FAILURE);
    }
    Read_Encoded_Bytes(encoded_file, size_of_file, bytes);


    Decode_Encoded_File(size_of_file, padding_zeroes, bytes, Node_list);


    fclose(encoded_file);
    

    return 0;
}
