#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    for (int j=n; j>=index_to_insert; j--) {
        Node_List[j+1] = Node_List[j];
    }
}


void Visit_Tree_To_Get_Encoding(struct Node Node_to_visit, int conversion_array[256][9], int* current_bin_code, int n_curr_bit_code, int bit_to_add) {
    //For every node (except the head of the tree), add a bit to the binary code
    if (bit_to_add != -1) {
        current_bin_code[0] ++;
        current_bin_code[n_curr_bit_code] = bit_to_add;
        n_curr_bit_code ++;
    } else {
        current_bin_code[0] = 0;
        n_curr_bit_code ++;
    }
    if (Node_to_visit.left != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.left, conversion_array, current_bin_code, n_curr_bit_code, 0);
    }
    if (Node_to_visit.right != NULL) {
        Visit_Tree_To_Get_Encoding(*Node_to_visit.right, conversion_array, current_bin_code, n_curr_bit_code, 1);
    }
    if (Node_to_visit.right == NULL && Node_to_visit.left == NULL) {
        //We add the binary code at the correct index (corresponding to the value of the byte represented)
        conversion_array[(int) (Node_to_visit.byte)][0] = n_curr_bit_code-1;
        for (int i=1; i<9; i++) {
            conversion_array[(int) (Node_to_visit.byte)][i] = current_bin_code[i];
        }
    }
}


int main()
{
    FILE *fp = fopen("test.png", "rb");

    //Get the byte length of the file
    fseek(fp, 0, SEEK_END);
    int size_of_file = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("Size of the file (in bytes) : %d", size_of_file);

    unsigned char *bytes;
    bytes = (unsigned char*) calloc(size_of_file, 1);

    //Read the bytes of the file
    fread(bytes, sizeof(unsigned char), size_of_file, fp);

    int n_diff_bytes = 0;
    unsigned char diff_bytes[256];
    //Counting all the different bytes of the file and storing them in diff_bytes
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
    printf("\nDifferent bytes : %d", n_diff_bytes);

    //Creating a frequencies array (same indexes as the diff_bytes array)
    int *frequencies;
    frequencies = (int*) calloc(n_diff_bytes, sizeof(int));

    //Going through each different byte to calculate how many time we find it in bytes
    for (int i=0; i<n_diff_bytes; i++) {
        frequencies[i] = 0;
        for (int j=0; j<size_of_file; j++) {
            if (diff_bytes[i] == bytes[j]) {
                frequencies[i] ++;
            }
        }
    }

    //(Double) Bubble sort by frequencies of diff_bytes and frequencies (the two arrays remain "linked")
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
        struct Node Sum_Node = Create_Node((unsigned char) 0, Node_list[0].weight + Node_list[1].weight, &memory_nodes[n_memory_nodes-2], &memory_nodes[n_memory_nodes-1]);
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

    /*int** conversion_array;
    conversion_array = (int**) calloc(256, sizeof(int*));
    if (conversion_array == NULL) {
        printf("Failed to allocate memory for conversion_array.\n");
        return 1; // or handle the error appropriately
    }

    for (int i=0; i<256; i++) {
        //At each index from 0 to 255, array of 9 int (first is the effective size of the binary code, max 8 binary digits)
        conversion_array[i] = (int*) calloc(9, sizeof(int));
    }
    */
    int conversion_array[256][9];
    int initial_bit_code[9];
    Visit_Tree_To_Get_Encoding(Node_list[0], conversion_array, initial_bit_code, 0, -1);

    printf("\n");
    for (int i=0; i<255; i++) {
        printf("%d ", conversion_array[i][0]);
    }


    /*for (int i=0; i<256; i++) {
        free(conversion_array[i]);
    }*/
    free(Node_list);
    free(memory_nodes);
    //free(conversion_array);
    free(bytes);
    fclose(fp);
    
    return 0;
}