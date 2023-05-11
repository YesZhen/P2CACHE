#include <linux/rbtree.h>

struct log_index_node {
    unsigned long key;
    void* ptr;
    struct rb_node nodes;
};

int log_index_insert_node(struct rb_root *tree_root, struct log_index_node *item);
