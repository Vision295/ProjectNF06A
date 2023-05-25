#include <stdio.h>
#include <stdlib.h>




struct Node{
    int weight;
    unsigned char byte;
    struct Node *right;
    struct Node *left;
};

struct Node Create_Node (unsigned char byte, int weight, struct Node* left, struct Node* right) {
    struct Node node;
    node.weight = weight;
    node.byte = byte;
    node.right = right;
    node.left = left;
    return node;
}

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
    Node_List[index_to_insert] = Node_Memory;

}


void Visit_Tree_To_Get_Encoding(struct Node Node_to_visit, unsigned short conversion_array[256][2], unsigned short current_bin_code, unsigned char n_bits_curr_bit_code, int bit_to_add) {
    int binary_code = 0;
    //Depending of the bit_to add, we add it to the current bin code
    if (bit_to_add == 0) {
        binary_code = current_bin_code<<1;
        //printf("\n%d %d", current_bin_code, current_bin_code<<1);
        n_bits_curr_bit_code ++;
    } else if (bit_to_add == 1) {
        binary_code = current_bin_code<<1 | 1;
        //printf("\n%d %d", current_bin_code, current_bin_code<<1 | 1);
        n_bits_curr_bit_code ++;
    }
    //If we are not yet at a leaf, we continue to explore the left and right side of the Node
    if (Node_to_visit.left != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.left, conversion_array, binary_code, n_bits_curr_bit_code, 0);
    }
    if (Node_to_visit.right != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.right, conversion_array, binary_code, n_bits_curr_bit_code, 1);
    }
    if (Node_to_visit.right == NULL && Node_to_visit.left == NULL) {
        //We add the binary code at the correct index (corresponding to the value of the byte represented)
        //conversion_array[i][0] is the size (in bits) of the code, and conversion_array[i][1] is the code in itself
        conversion_array[(int) (Node_to_visit.byte)][0] = n_bits_curr_bit_code;
        conversion_array[(int) (Node_to_visit.byte)][1] = binary_code;

        printf("\n %u : coded in %d bits : %d", Node_to_visit.byte, n_bits_curr_bit_code, binary_code);
    }
}






int main()
{

    //File to read from, so that we can rebuild the Huffman Tree
    FILE *conversion_file = fopen("conversion.bin", "rb");

    //Create the structures to read the file
    int n_diff_bytes;
    unsigned char diff_bytes[256];

    n_diff_bytes = getw(conversion_file);
    fread(diff_bytes, sizeof(unsigned char), 256, conversion_file);

    //We have to allocate the memory depending of the first int read
    int *frequencies;
    frequencies = (int*) calloc(n_diff_bytes, sizeof(int));

    fread(frequencies, sizeof(int), n_diff_bytes, conversion_file);




    //Creates the list of Nodes to add into the tree
    struct Node* Node_list = (struct Node*) calloc(n_diff_bytes, sizeof(struct Node));
    for (int i=0; i<n_diff_bytes; i++) {
        Node_list[i] = Create_Node(diff_bytes[i], frequencies[i], NULL, NULL);
    }

    //Creating a memory list of copy_nodes to store the ones we erase from Node_List, to keep track of them 
    struct Node* memory_nodes = (struct Node*) calloc(256, sizeof(struct Node));
    int n_memory_nodes = 0;


    //Repeat the following instructions until we only have one Node left, the "top" of the tree
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

    printf("\nThe top Node has a weight of : %d", Node_list[0].weight);

    unsigned short conversion_array[256][2];
    unsigned short initial_bit_code = 0;
    Visit_Tree_To_Get_Encoding(Node_list[0], conversion_array, initial_bit_code, 0, -1);





    FILE *encoded_file = fopen("output.bin", "rb");

    //Get the byte length of the file
    fseek(encoded_file, 0, SEEK_END);
    int size_of_file = ftell(encoded_file);
    fseek(encoded_file, 0, SEEK_SET);

    //Get number of padding zeroes in the penultimate byte
    fseek(encoded_file, -1, SEEK_END);
    unsigned char padding_zeroes;
    fread(&padding_zeroes, sizeof(unsigned char), 1, encoded_file);

    fseek(encoded_file, 0, SEEK_SET);

    printf("Size of the file (in bytes) : %d", size_of_file);

    unsigned char *bytes;
    bytes = (unsigned char*) calloc(size_of_file, 1);
    if (bytes == NULL) {
        printf("\n bytes failed to load");
        exit(EXIT_FAILURE);
    }

    //Read the bytes of the file
    fread(bytes, sizeof(unsigned char), size_of_file, encoded_file);




    FILE *decoded_file = fopen("decoded.bin", "wb");


    //Index through the array bytes
    int n_bytes = 0;
    //How many bits of the actual byte we have taken off
    int n_bytes_i = 0;

    struct Node Node_Visiting = Node_list[0];


    while (n_bytes < size_of_file-1) {

        //If we've just reached the penultimate byte (the last is the number of padding zeroes)
        if (n_bytes == size_of_file-2 && n_bytes_i == 0){
            //Make sure we won't read the padding zeroes at the end of the byte
            //Doing this, we will read 8-padding_zeroes bits of this byte
            n_bytes_i = padding_zeroes;
        }
   
        if (bytes[n_bytes] >=128) {
            Node_Visiting = *Node_Visiting.right;
        } else {
            Node_Visiting = *Node_Visiting.left;
        }
        //Shift to the left
        bytes[n_bytes] = bytes[n_bytes] <<1;
        n_bytes_i++;

        //Go to the next byte
        if (n_bytes_i == 8) {
            n_bytes_i = 0;
            n_bytes++;
        }

        if (Node_Visiting.left == NULL && Node_Visiting.right == NULL) {
            putc(Node_Visiting.byte, decoded_file);
            //Go back to the head of the tree
            Node_Visiting = Node_list[0];
        }

    }


    fclose(conversion_file);
    fclose(encoded_file);
    fclose(decoded_file);

    return 0;
}
