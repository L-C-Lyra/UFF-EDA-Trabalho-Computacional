#include "bplus_tree_io.h"



int main()
{
    FILE *fp = fopen("indices_t.bin", "wb");
    init_BPT(fp);
    fclose(fp);

    BPTHeader *file_header = (BPTHeader *)malloc(sizeof(BPTHeader));
    LeafNode *leaf_node = (LeafNode *)malloc(sizeof(LeafNode));
    fp = fopen("indices_t.bin", "rb");

    read_header(fp, file_header);
    print_header(file_header);

    printf("\n");
    print_internal_nodes(fp);

    read_leaf_node(-1, leaf_node);
    print_leaf_aux(leaf_node);
}