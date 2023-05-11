#include "log-index.h"

int log_index_insert_node(struct rb_root *tree_root, struct log_index_node *item)
{
    struct rb_node **inserted_point = &(tree_root->rb_node);
    struct rb_node *parent = NULL;

    while(*inserted_point) {
        struct log_index_node *tmp_item = rb_entry(*inserted_point, struct log_index_node, nodes);
        parent = *inserted_point;

        if(item->key < tmp_item->key)
            inserted_point = &((*inserted_point)->rb_left);
        else if(item->key > tmp_item->key)
            inserted_point = &((*inserted_point)->rb_right);
        else
            return 1;
    }

    rb_link_node(&item->nodes, parent, inserted_point);
    rb_insert_color(&item->nodes, tree_root);
    return 0;
}