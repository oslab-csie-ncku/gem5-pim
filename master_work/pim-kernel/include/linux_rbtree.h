#ifndef __LINUX_RBTREE_H__
#define __LINUX_RBTREE_H__

struct rb_node
{
    unsigned long  __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root
{
    struct rb_node *rb_node;
};

#endif /* __LINUX_RBTREE_H__ */
