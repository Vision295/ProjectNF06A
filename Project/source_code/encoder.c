/**
 * @file encoder.c
 * @author Lucas SCHUMMER
 * @brief The file encoder that uses the Huffman Coding Algorithm
 * @version 1.0
 * @date 28/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <limits.h>


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


/**
 * @brief           Write a file that will be read by the decoder. It is a text file containing the name of the encoded file, so that
 *                  the decoder will be able to open it
 * 
 * @param filename  The name of the file (string)
 */
void Write_Filename(char* filename) {

    FILE* file = fopen("../compressed/compressed.txt", "wb");
    fputs(filename, file);
    fclose(file);

}


/**
 * @brief               Read the file to encode and store its bytes into an array
 * 
 * @param size_of_file  The number of bytes of the file (which is also the length of the array of bytes)
 * @param bytes         The dynamically allocated bytes array, where we store the data from the file to compress
 * @param fp            The file to compress, already open in read(binary) mode
 */
void Read_Bytes(int size_of_file, unsigned char* bytes, FILE* fp) {

    if (bytes == NULL) {
        printf("\n bytes failed to load");
        exit(EXIT_FAILURE);
    }    
    fread(bytes, sizeof(unsigned char), size_of_file, fp);

}


/**
 * @brief               From the read bytes of the file to compress, extract all the different bytes present and get the amount of different bytes
 * 
 * @param size_of_file  The size of the file (the length of the bytes array)
 * @param bytes         Array of bytes read from the file to compress
 * @param diff_bytes    Array of bytes containing all the different bytes we've read from the file
 * 
 * @return              The amount (integer) of different bytes in the file (from 0 to 256)
 */
int Get_Diff_Bytes(int size_of_file, unsigned char* bytes, unsigned char* diff_bytes) {

    int n_diff_bytes = 0;
    for (int i=0; i<size_of_file; i++) {

        int is_in_diff_bytes = 0;
        int j=0;
        while ((j<n_diff_bytes) && (is_in_diff_bytes==0)) {
            if (bytes[i] == diff_bytes[j]) {
                is_in_diff_bytes = 1;
            }
            j++;
        }
        
        if (is_in_diff_bytes == 0) {
            diff_bytes[n_diff_bytes] = bytes[i];
            n_diff_bytes++;
        }
    }
    return n_diff_bytes;

}


/**
 * @brief               From the bytes of the file and the list of different bytes, count the number of occurencies of each byte in the whole file
 * 
 * @param size_of_file  The size of the file (the length of the bytes array)
 * @param n_diff_bytes  The number of different bytes (the length of the diff_bytes array)
 * @param bytes         All the bytes of the file to compress
 * @param diff_bytes    All the different bytes present in the file
 * @param frequencies   Array of integers to store the number of occurencies of each byte. It is of length n_diff_bytes (max 256)
 *                      The frequencies and diff_bytes arrays are "symetrical" : For any index i between 0 and n_diff_bytes-1 
 *                      diff_bytes[i] is the value of the byte, and frequencies[i] how many times you find it in the file
 *                          
 */
void Get_Bytes_Frequencies(int size_of_file, int n_diff_bytes, unsigned char* bytes, unsigned char* diff_bytes, int* frequencies) {

    //Going through each different byte to calculate how many time we find it in bytes
    for (int i=0; i<n_diff_bytes; i++) {
        frequencies[i] = 0;
        for (int j=0; j<size_of_file; j++) {
            if (diff_bytes[i] == bytes[j]) {
                frequencies[i] ++;
            }
        }
    }

}


/**
 * @brief               A kind of "double" bubble sort : we sort diff_bytes and frequencies by frequencies(ascending order). The arrays stay "symetrical"
 *                      frequencies[i] still corresponds to the occurencies of the byte diff_bytes[i] in the file     
 * 
 * @param n_diff_bytes  The number of different bytes (the length of the diff_bytes array)
 * @param diff_bytes    All the different bytes present in the file
 * @param frequencies   The number of occurencies of each byte present in the file
 */
void Bubble_Sort(int n_diff_bytes, unsigned char* diff_bytes, int* frequencies) {

    int Modif = 1;
    int i=0;

    while (Modif == 1 && i<n_diff_bytes-1) {
        Modif = 0;
        for (int j=0; j<n_diff_bytes-i-1; j++) {
            if (frequencies[j] > frequencies[j+1]) {
                Modif = 1;
                int memory = frequencies[j+1];
                frequencies[j+1] = frequencies[j];
                frequencies[j] = memory;
                unsigned char memory2;
                memory2 = diff_bytes[j+1];
                diff_bytes[j+1] = diff_bytes[j];
                diff_bytes[j] = memory2;
            }
        }
        i++;
    }

}


/**
 * @brief               Write the file to transfer to the decoder (in addition to the compressed file)
 *                      This file contains the information necessary for the decoder to rebuild the tree
 * 
 * @param n_diff_bytes  The number of different bytes (the length of the diff_bytes array)
 * @param diff_bytes    All the different bytes present in the file
 * @param frequencies   The number of occurencies of each byte present in the file
 */
void Write_Compression_File (int n_diff_bytes, unsigned char* diff_bytes, int* frequencies) {

    FILE *conversion_file = fopen("../compressed/conversion.bin", "wb");
    putw(n_diff_bytes, conversion_file);
    fwrite(diff_bytes, sizeof(unsigned char), 256, conversion_file);
    fwrite(frequencies, sizeof(int), n_diff_bytes, conversion_file);

    fclose(conversion_file);

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

    //Repeat the following instructions until we only have one Node left, the "top" of the tree
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
 * @brief                       A recursive fonction used to visit the entire Huffman Tree once it's been constructed, so that we can get 
 *                              the encoding of each different byte (the encoding of a byte corresponds to the path from the "head" of the tree
 *                              to the struct Node (a leaf of the tree) containing the byte)
 * 
 * @param Node_to_visit         The struct Node we are currently visiting. Initially, it is the "head" of the tree 
 *                              (i.e the last element in the Nodes_List while building the tree)
 * @param conversion_array      A 2D array where we store the encoding of each byte. We store first the bit size of the encoding 
 *                              (i.e the number of Nodes we've went through before reaching the corresponding Node), and then the encoding in itself (over 2 bytes)
 *                              Example : At conversion_array[10][1] you will find the encoding for the byte 00001010 (10 in binary) which can be:
 *                              00000000 00101110 . And at conversion_array[0] for example 7 (we only have to take into account the last 7 bits)
 *                              
 * @param current_bin_code      Going from the head of the tree, we add a 0 each time we go left down the tree, and a one if we go right
 *                              current_bin_code contains the history of the way we've been through up until now
 *                              When eventually we will reach a leaf, the current_bin_code will become the encodind of the corresponding byte
 *                              (the byte stored by the Node at Node.byte)
 * @param n_bits_curr_bit_code  The length of the current binary code (i.e the number of time we've went left or right up until now)
 * @param bit_to_add            The bit to add to the current binary code (i.e 0 if we've just went left, 1 if it was right)
 */
void Visit_Tree_To_Get_Encoding(struct Node Node_to_visit, unsigned short conversion_array[256][2], unsigned short current_bin_code, unsigned char n_bits_curr_bit_code, int bit_to_add) {
    
    int binary_code = 0;
    //Depending of the bit_to add, we add it to the current bin code
    if (bit_to_add == 0) {
        binary_code = current_bin_code<<1;
        n_bits_curr_bit_code ++;
    } else if (bit_to_add == 1) {
        binary_code = current_bin_code<<1 | 1;
        n_bits_curr_bit_code ++;
    }

    //If we are not yet at a leaf, we continue to explore the left and right side of the Node
    if (Node_to_visit.left != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.left, conversion_array, binary_code, n_bits_curr_bit_code, 0);
    }
    if (Node_to_visit.right != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.right, conversion_array, binary_code, n_bits_curr_bit_code, 1);
    }

    //Here, we've reached a leaf of the tree
    if (Node_to_visit.right == NULL && Node_to_visit.left == NULL) {
        //We add the binary code at the correct index (corresponding to the value of the byte represented)
        //conversion_array[i][0] is the size (in bits) of the code, and conversion_array[i][1] is the code in itself
        conversion_array[(int) (Node_to_visit.byte)][0] = n_bits_curr_bit_code;
        conversion_array[(int) (Node_to_visit.byte)][1] = binary_code;
    }

}


/**
 * @brief                   Convert each byte of the file into its compressed form and write the whole into the compressed file
 * 
 * @param size_of_file      The size of the file (the length of the bytes array)
 * @param bytes             All the bytes of the file to compress
 * @param conversion_array  The array that enables us to convert each byte into its compressed form
 */
void Write_Encoded_To_File(int size_of_file, unsigned char* bytes, unsigned short conversion_array[256][2]) {

    FILE* output_file;
    output_file = fopen("../compressed/encoded.bin", "wb");


    //Set buffer to 00000000
    unsigned char buffer = 0;
    int len_buffer = 0;
    for (int i=0; i<size_of_file; i++) {
        unsigned short encoded = conversion_array[bytes[i]][1];
        //encoded is written on n bits, but the written bits are at the end of the two bytes (unsigned short). We shift them to the beginning of the two byte for easier computations
        encoded = encoded<<(16-conversion_array[bytes[i]][0]);
    
        for (int j=0; j<conversion_array[bytes[i]][0]; j++) {

            //Add the first bit of encoded to the buffer (32768=2^15)
            if (encoded >= 32768) {
                buffer = (buffer<<1) | 1;
                len_buffer ++;
            } else {
                buffer = buffer<<1;
                len_buffer ++;
            }

            //Erase the first bit of encoded
            encoded = encoded<<1;

            //Write into the file when the buffer is full
            if (len_buffer == 8) {
                fputc(buffer, output_file);
                buffer = 0;
                len_buffer = 0;
            }
        }
    }

    //If there are som bits still not in the file (i.e if the number of bits encoded is not divisible by 8)
    if (len_buffer > 0) {
        unsigned char padding_count = 0;
        //Adding zeros at the end until we have a full byte
        while (len_buffer < 8) {
            buffer = buffer<<1;  
            len_buffer++;
            padding_count++;
        }
        //Writing the filled byte as well as the number of padding zeros, to be able to remove them while decompressing
        fputc(buffer, output_file);
        fputc(padding_count, output_file);
    } else {
        //Even though we don't have any padding zeroes to add, we write the number of padding zeroes (i.e 0) to the last byte
        fputc((unsigned char) 0, output_file);
    }

    fclose(output_file);

}


/**
 * @brief   Read from file_to_compress.txt the name of the file to compress, open this file and read its bytes. 
 *          Create the Huffman Tree according to the Huffman Coding Algorithm. Go through the tree to get the bit sequence corresponding 
 *          to the each byte. Write the compressed version of each byte into compressed.bin, as well as the information necessary for the decoder
 *          to rebuild the tree in conversion.bin
 * 
 * @return  0 if there were no errors in the process
 */
int main()
{

    //Open the file created by the python program that tells which file should we compress
    FILE* filename_file = fopen("../compressed/file_to_compress.txt", "r");
    if (filename_file == NULL) {
        printf("\nFile failed to open");
    }
    char* filename;
    fscanf(filename_file, "%s", filename);

    //Write the name of the file we've compressed in a text file
    Write_Filename(filename);


    fclose(filename_file);

    //Generate the correct path to the file to compress and open it in read mode
    char path[] = "../gallery/";
    strcat(path, filename);

    FILE* fp = fopen(path, "rb");


    //Get the byte length of the file
    fseek(fp, 0, SEEK_END);
    int size_of_file = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    //Read the bytes of the file
    unsigned char *bytes;
    bytes = (unsigned char*) calloc(size_of_file, 1);

    Read_Bytes(size_of_file, bytes, fp);

    fclose(fp);


    unsigned char diff_bytes[256];
    int n_diff_bytes = Get_Diff_Bytes(size_of_file, bytes, diff_bytes);

    //Creating a frequencies array (same indexes as the diff_bytes array)
    int *frequencies;
    frequencies = (int*) calloc(n_diff_bytes, sizeof(int));

    Get_Bytes_Frequencies(size_of_file, n_diff_bytes, bytes, diff_bytes, frequencies);

    Bubble_Sort(n_diff_bytes, diff_bytes, frequencies);
    
    Write_Compression_File(n_diff_bytes, diff_bytes, frequencies);
 
    //Creates the list of Nodes to add into the tree
    struct Node* Node_list = (struct Node*) calloc(n_diff_bytes, sizeof(struct Node));
    for (int i=0; i<n_diff_bytes; i++) {
        Node_list[i] = Create_Node(diff_bytes[i], frequencies[i], NULL, NULL);
    }

    //Creating a memory list of copy_nodes to store the ones we erase from Node_List, to keep track of them
    //The tree can contain up to 512 nodes (the leaves + all the ones built from the sum of the two under) 
    struct Node* memory_nodes = (struct Node*) calloc(512, sizeof(struct Node));

    Create_Huffman_Tree(n_diff_bytes, Node_list, memory_nodes);

    //Creating the array that stores the compressed form of each byte. We then fill it in the function Visit_Tree_To_Get_Encoding()
    //For more details about conversion_array, see Visit_Tree_To_Get_Encoding()
    unsigned short conversion_array[256][2];

    unsigned short initial_bit_code = 0;
    Visit_Tree_To_Get_Encoding(Node_list[0], conversion_array, initial_bit_code, 0, -1);

    Write_Encoded_To_File(size_of_file, bytes, conversion_array);

    //Free dynamically allocated arrays
    free(frequencies);
    free(Node_list);
    free(memory_nodes);
    free(bytes);
    
    
    return 0;
}