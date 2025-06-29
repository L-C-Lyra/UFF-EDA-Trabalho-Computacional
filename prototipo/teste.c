#include "bplus_tree_io.h"
#include "bplus_tree.h"

int main()
{
    FILE *fp = fopen("indices_t.bin", "wb+");
    init_BPT(fp, BTREE_ORDER);

    PlayerData players[15];

    char letters[15][2] = {
        "A", "B", "C", "D", "E", "F", "H", "I", "J", "K", "L", "P", "X", "Y", "Z"};

    
    for (int i = 0; i < 15; i++) {
        memset(&players[i], 0, sizeof(PlayerData));
        strncpy(players[i].name, letters[i], NAME_SIZE);
        players[i].birth_year = i * 10;
    }
    
    // Nó 1
    InternalNode n1;
    memset(&n1, 0, sizeof(InternalNode)); 
    n1.num_keys = 1;
    strcpy(n1.keys[0], players[8].name); // Copia chave "J"
    n1.children_pointers[0] = sizeof(BPTHeader) + (1 * sizeof(InternalNode));
    n1.children_pointers[1] = sizeof(BPTHeader) + (2 * sizeof(InternalNode));;

    // Nó 2
    InternalNode n2;
    memset(&n2, 0, sizeof(InternalNode));
    n2.num_keys = 1;
    strcpy(n2.keys[0], players[4].name); // Copia a chave "E"
    n2.children_pointers[0] = -1;
    n2.children_pointers[1] = -2;

    // Nó 3
    InternalNode n3;
    memset(&n3, 0, sizeof(InternalNode));
    n3.num_keys = 1;
    strcpy(n3.keys[0], players[11].name); // Copia a chave "P"
    n3.children_pointers[0] = -3;
    n3.children_pointers[1] = -4;

    write_internal_node_from_id(fp, 0, &n1);
    write_internal_node_from_id(fp, 1, &n2);
    write_internal_node_from_id(fp, 2, &n3);

    BPTHeader *header = read_header(fp);
    header->internal_node_count = 3;
    header->root_offset = 24;
    header->leaf_count = 4;
    write_header(fp, header);

    LeafNode l1 = {4, 0, 2, {players[0], players[1], players[2], players[3]}};
    LeafNode l2 = {4, 1, 3, {players[4], players[5], players[6], players[7]}};
    LeafNode l3 = {3, 2, 4, {players[8], players[9], players[10]}};
    LeafNode l4 = {4, 3, 5, {players[11], players[12], players[13], players[14]}};

    write_leaf_node(n2.children_pointers[0], &l1);
    write_leaf_node(n2.children_pointers[1], &l2);
    write_leaf_node(n3.children_pointers[0], &l3);
    write_leaf_node(n3.children_pointers[1], &l4);

    fclose(fp);
    printf("DEBUG\n");
    fp = fopen("indices_t.bin", "rb+");

    header = read_header(fp);
    print_header(header);
    print_internal_nodes(fp);

    PlayerData *data = bpt_search(fp, "H");
    printf("\n");
    print_player(data);
    fclose(fp);

    free(data);
    free(header);
}