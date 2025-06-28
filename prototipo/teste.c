#include "bplus_tree_io.h"



int main()
{
    FILE *fp = fopen("indices_t.bin", "wb+");
    init_BPT(fp);

    PlayerData player1 = {"Ivan", "Lendl", 1960, -1, "United States" ,1, 1985, 100};
    PlayerData player2 = {"Andres", "Gomez", 1960, -1, "Ecuador", 4, 1990, 100};
    PlayerData player3 = {"John", "Fitzgerald", 1960, -1, "Australia", 25, 1988, 100};
    LeafNode node1 = {3, 0, 0, {player1, player2, player3}};

    PlayerData player4 = {"Greg", "Holmes", 1963, -1, "United States", 22, 1985};
    PlayerData player5 = {"Francesco", "Cancellotti", 1963, -1, "Italy", 21, 1985};
    LeafNode node2 = {2, 0, 0, {player4, player5}};

    add_leaf_node(fp, &node1);
    add_leaf_node(fp, &node2);
    fclose(fp);

    BPTHeader *file_header = (BPTHeader *)malloc(sizeof(BPTHeader));
    LeafNode *leaf_node = (LeafNode *)malloc(sizeof(LeafNode));
    fp = fopen("indices_t.bin", "rb");

    read_header(fp, file_header);
    print_header(file_header);
    printf("\n");
    print_internal_nodes(fp);

    print_leafs(file_header);
}