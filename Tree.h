//header with tree

struct elem_t
    {
    char *ptr = nullptr;
    bool memory = true;
    };

const elem_t def_init_f = {nullptr, false};
const elem_t def_init_t = {nullptr, true};
const char RIGHT = 1;
const char LEFT = 0;
const char NAN_NODE = -1;
const int MAX_LENGTH_OF_NAME = 64;
const char DEF_LENGTH_OF_PHRASE = 64;
const char directory_name[] = "../";
const char dump_file_name[] = "DUMP_TREE_";
const char saving_file_name[] = "SAVED_TREE_";
const char dump_format[] = ".txt";
const char def_input[] = "../source_tree.txt";
const char for_stdin[] = "\n%[^\n]";
const char open_sym = '{';
const char close_sym = '}';
const char con_sym = '\"';

#define for_treedump __FILE__,__PRETTY_FUNCTION__,__LINE__

#define tree_assert(arg) assert(arg)
#define nil nullptr
#define def_init { (char*)calloc(DEF_LENGTH_OF_PHRASE, sizeof(char) ), true };

struct node_t
    {
    elem_t val = def_init_t;
    node_t *left = nil;
    node_t *right = nil;
    node_t *up = nil;
    };

bool node_init(node_t *node)
    {
    tree_assert(node);

    node->val = def_init_t;
    node->right = nil;
    node->left = nil;
    node->up = nil;

    return true;
    }

bool elem_verify(const elem_t elem)
    {
    if (elem.memory)
        {
        if ( elem.ptr == nullptr )
            {
            return false;
            }
        }

    return true;
    }

bool node_verify(const node_t *node)
    {
    tree_assert(node);

    bool res = true;
    res = res && elem_verify(node->val);
    if (res && (node->left != nil) )
        {
        res = res && (node == (node->left->up));
        }
    if (res && (node->right != nil) )
        {
        res = res && (node == (node->right->up));
        }
    if (res && (node->up != nil) )
        {
        res = res && ( (node == (node->up->right)) || (node == (node->up->left)) );
        }

    return res;
    }

bool tree_verify(const node_t *rt_node)
    {
    if (rt_node != nil)
        {
        node_verify(rt_node);
        tree_verify(rt_node->left);
        tree_verify(rt_node->right);
        }

    return true;
    }

node_t *create_node(node_t *parent = nil, char son = NAN_NODE, elem_t value = def_init_t)
    {
    node_t *node = (node_t *) calloc(1, sizeof(node_t));
    node->left = nil;
    node->right = nil;
    node->up = parent;
    node->val = value;
    if (node->val.memory)
        {
        node->val.ptr = (char *) calloc(DEF_LENGTH_OF_PHRASE, sizeof(char));
        strcpy(node->val.ptr, value.ptr );
        }
    if (parent != nil)
        {
        if (son == RIGHT)
            {
            parent->right = node;
            }
        if (son == LEFT)
            {
            parent->left = node;
            }
        }

    tree_assert(node_verify(node));
    return node;
    }

void elem_delete(elem_t elem)
    {
    if (elem.memory)
        {
        free(elem.ptr);
        }
    }

bool tree_delete(node_t *node)
    {
    tree_assert(tree_verify(node));

    if (node->left != nil)
        {
        tree_delete(node->left);
        }
    if (node->right != nil)
        {
        tree_delete(node->right);
        }

    elem_delete(node->val);
    free(node);
    return true;
    }

bool elem_print(FILE *file, elem_t elem)
    {
    tree_assert(file);

    fprintf(file, "CONTENT:\nfield->ptr: %s\nfield->Allocated: %d\n", elem.ptr, elem.memory);
    return true;
    }

bool node_dump(const node_t *node, FILE *file)
    {
    tree_assert(node);
    tree_assert(file);

    fprintf(file, "\nONE MORE node:\naddress [%p]\n", node);
    elem_print(file, node->val);
    fprintf(file, "\nLEFT SON: [%p]\n", node->left);
    fprintf(file, "\nRIGHT SON: [%p]\n", node->right);
    fprintf(file, "\nPARENT: [%p]\n", node->up);

    return true;
    }

bool node_dot_print(const node_t *node, FILE *dot, bool s1mple_mode = false)
    {
    tree_assert(node);
    tree_assert(dot);

    if (s1mple_mode)
        {
        fprintf(dot, "NODE%p[ shape = record, shape = \"polygon\" label = \" %s \" ];\n", node, node->val.ptr);
        return true;
        }
    fprintf(dot,
            "NODE%p[ shape = record, label = \" NODE: %p | { ELEM: %s | Allocated: %d } | { LEFT SON: %p | RIGHT SON: %p | "
            "PARENT: %p } \" ];\n", node, node, node->val.ptr, node->val.memory, node->left, node->right, node->up);

    return true;
    }

bool node_dump_rec(const node_t *node, FILE *file)
    {
    tree_assert(node);
    tree_assert(file);

    node_dump(node, file);
    if (node->left != nil)
        {
        node_dump_rec(node->left, file);
        }
    if (node->right != nil)
        {
        node_dump_rec(node->right, file);
        }

    return true;
    }

char tree_fam_relshp(const node_t *node_p, const node_t *node_c)
    {
    tree_assert(node_p && node_c);

    if (node_c == node_p->left)
        {
        return LEFT;
        }
    if (node_c == node_p->right)
        {
        return RIGHT;
        }

    return NAN_NODE;
    }

bool node_dot_rec(const node_t *node, FILE *dot, bool s1mple_mode = false)
    {
    tree_assert(node);
    tree_assert(dot);

    node_dot_print(node, dot, s1mple_mode);
    if (node->up != nil)
        {
        if (s1mple_mode)
            {
            if (tree_fam_relshp(node->up, node) == RIGHT)
                {
                fprintf(dot, "NODE%p->NODE%p[ label = \"YES\", color = \"blue\" ];\n", node->up, node);
                }
            if (tree_fam_relshp(node->up, node) == LEFT)
                {
                fprintf(dot, "NODE%p->NODE%p[ label = \"NO\", color = \"blue\" ];\n", node->up, node);
                }
            } else
            {
            fprintf(dot, "NODE%p->NODE%p[ color = \"blue\" ];\n", node->up, node);
            }
        }
    if (node->left != nil)
        {
        node_dot_rec(node->left, dot, s1mple_mode);
        }
    if (node->right != nil)
        {
        node_dot_rec(node->right, dot, s1mple_mode);
        }

    return true;
    }

bool tree_dot_dump(const node_t *node, const char tree_name[] = "", bool open = false, bool s1mple_mode = false)
    {
    tree_assert(node);

    char dot_name[MAX_LENGTH_OF_NAME] = "";
    strcat(strcat(dot_name, directory_name), "TREE_dump_dot_");
    strcat(dot_name, tree_name);
    strcat(dot_name, ".txt");
    FILE *dot = fopen(dot_name, "w");
    fprintf(dot,
            "digraph G\n{\nrankdir = TB;\nnode[ color = \"goldenrod1\",fontsize = 10, shape = \"doublecircle\" ];\n");
    node_dot_rec(node, dot, s1mple_mode);
    fprintf(dot, "}");
    fclose(dot);

    char dot_out[2 * MAX_LENGTH_OF_NAME] = "dot -Tps ";
    strcat(strcat(dot_out, dot_name), " -o ");
    strcat(strcat(dot_out, dot_name), ".PDF");
    system(dot_out);
    if (open)
        {
        strcpy(dot_out, "gio open ");
        strcat(strcat(dot_out, dot_name), ".PDF");
        system(dot_out);
        }

    return true;
    }

bool
tree_dump(const node_t *node, const char tree_name[] = "", const char s1[] = "NS", const char s2[] = "NS", int line = 0,
          const char full_name[] = "")
    {
    tree_assert(node);

    char file_name[MAX_LENGTH_OF_NAME] = "";
    if (strcmp(full_name, "") != 0)
        {
        strcpy(file_name, full_name);
        } else
        {
        strcat(file_name, directory_name);
        strcat(file_name, dump_file_name);
        strcat(file_name, tree_name);
        strcat(file_name, dump_format);
        }
    FILE *dump = fopen(file_name, "w");

    fprintf(dump, "NAME OF TREE: %s\n", tree_name);
    fprintf(dump, "In file %s | In function %s | line %d\n", s1, s2, line);
    node_dump_rec(node, dump);

    fclose(dump);
    return true;
    }

bool node_get_rec(node_t *node, char **buffer)
    {
    tree_assert(buffer);
    char arg[DEF_LENGTH_OF_PHRASE] = "";
    int shift = 0;
    (*buffer)++;
    switch (*(*buffer - 1))
        {

        case open_sym :
            {
            sscanf(*buffer, " \"%[^\"]\" %n", arg, &shift);
            (*buffer) += shift;
            node->val.ptr = strdup(arg);
            if (**buffer == open_sym)
                {
                create_node(node, LEFT, def_init_f);
                node_get_rec(node->left, buffer);
                }
            if (**buffer == open_sym)
                {
                create_node(node, RIGHT, def_init_f);
                node_get_rec(node->right, buffer);
                }
            if (**buffer == close_sym)
                {
                (*buffer)++;
                return true;
                }
            }
            break;

        case close_sym :
            {
            (*buffer)++;
            return true;
            }
            break;

        case '\0' :
            {
            return true;
            }

        case '\n' :
            {
            return true;
            }

        default :
            {
            printf("ERROR in reading file with tree");
            exit(228);
            }

        }
    return true;
    }

node_t *tree_get_txt(const char file_name[] = def_input)
    {
    char *buffer = nullptr;
    long count_sym = 0;
    buffer_read(&buffer, &count_sym, file_name, 0, "r");
    node_t *root = create_node(nil, NAN_NODE, def_init_f);
    char *buffer_cpy = buffer;
    node_get_rec(root, &buffer);

    free(buffer_cpy);
    return root;
    }

void tree_out_rec(const node_t *node, FILE *save)
    {
    tree_assert(node_verify(node));
    tree_assert(save);

    fprintf(save, "{\"%s\"", node->val.ptr);
    if (node->left != nil)
        {
        tree_out_rec(node->left, save);
        }
    if (node->right != nil)
        {
        tree_out_rec(node->right, save);
        }
    fprintf(save, "}");
    }

void tree_out_txt(const node_t *node, const char tree_name[])
    {
    tree_assert(tree_verify(node));

    char out_name[MAX_LENGTH_OF_NAME] = "";
    strcat(out_name, directory_name);
    strcat(out_name, saving_file_name);
    strcat(out_name, tree_name);
    strcat(out_name, ".txt");
    FILE *save = fopen(out_name, "w");

    tree_out_rec(node, save);
    fclose(save);
    }

void tree_find_rec( node_t* node, elem_t* elem, node_t** res)
    {
    if ( *res == nil )
        {
        if ( strcasecmp( node->val.ptr, elem->ptr) == 0 )
            {
            *res = node;
            }
        if ( node->left != nil )
            {
            tree_find_rec( node->left, elem, res );
            }
        if ( node->right != nil )
            {
            tree_find_rec( node->right, elem, res );
            }
        }
    }

node_t* tree_find_by_value( node_t* node, elem_t* elem )
    {
    assert( tree_verify(node) );

    node_t* res = nil;
    tree_find_rec( node, elem, &res );
    return res;
    }