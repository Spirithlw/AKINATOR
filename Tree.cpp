#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "files.h"

struct elem_t
    {
    long val = 0;
    long* ptr = nullptr;
    };

const elem_t def_init = { 0, nullptr };
const char RIGHT = 1;
const char LEFT = 0;
const char NAN_NODE = -1;
const int MAX_LENGTH_OF_NAME = 64;
const char directory_name[] = "../";
const char dump_file_name[] = "DUMP_TREE_";
const char dump_format[] = ".txt";
const char def_input[] = "../source_tree.txt";
const char open_sym = '{';
const char close_sym = '}';
const char con_sym = '\"';

#define for_treedump __FILE__,__PRETTY_FUNCTION__,__LINE__

#define tree_assert( arg ) assert(arg)
#define nil nullptr

struct node_t
    {
    elem_t val = def_init;
    node_t* left = nil;
    node_t* right = nil;
    node_t* up = nil;
    };

bool node_init( node_t* node )
    {
    tree_assert( node );

    node->val = def_init;
    node->right = nil;
    node->left = nil;
    node->up = nil;
    return true;
    }

bool elem_verify( const elem_t elem )
    {
    tree_assert(elem.ptr);
    return true;
    }

bool node_verify( const node_t* node )
    {
    tree_assert(node);

    tree_assert( elem_verify( node->val ) );
    if ( node->left != nil )
        {
        tree_assert( node == (node->left->up) );
        }
    if ( node->right != nil )
        {
        tree_assert( node == (node->right->up) );
        }
    if ( node->up != nil )
        {
        tree_assert( ( node == (node->up->right) ) || ( node == (node->up->left) )  );
        }
    return true;
    }

bool tree_verify( const node_t* rt_node )
    {
    if ( rt_node != nil )
        {
        node_verify( rt_node );
        tree_verify(rt_node->left);
        tree_verify(rt_node->right);
        }
    return true;
    }

node_t* create_node( node_t* parent = nil, char son = NAN_NODE, elem_t value = def_init )
    {
    node_t* node = (node_t*) calloc(1,sizeof(node_t) );
    node->left = nil;
    node->right = nil;
    node->up = parent;
    node->val = value;
    node->val.ptr = (long*) calloc( 1, sizeof(long) );
    if ( parent != nil )
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

    return node;
    }

bool elem_delete(elem_t elem)
    {
    free(elem.ptr);
    return true;
    }

bool tree_delete( node_t* node )
    {
    tree_assert( tree_verify(node) );

    elem_delete( node->val );
    if ( node->left != nil )
        {
        tree_delete(node->left);
        }
    if ( node->right != nil )
        {
        tree_delete(node->right);
        }

    free(node);
    return true;
    }

bool elem_print( FILE* file, elem_t elem )
    {
    tree_assert(file);

    fprintf( file, "CONTENT:\nfield->val: %ld\n", *(elem.ptr) );
    return true;
    }

bool node_dump( const node_t* node, FILE* file )
    {
    tree_assert( node );
    tree_assert(file);

    fprintf( file, "\nONE MORE node:\naddress [%p]\n", node );
    elem_print( file, node->val );
    fprintf( file, "\nLEFT SON: [%p]\n", node->left );
    fprintf( file, "\nRIGHT SON: [%p]\n", node->right );
    fprintf( file, "\nPARENT: [%p]\n", node->up );

    return true;
    }

bool node_dot_print( const node_t* node, FILE* dot )
    {
    tree_assert(node);
    tree_assert(dot);

    fprintf( dot, "NODE%p[ shape = record, label = \" NODE: %p | ELEM: %ld | { LEFT SON: %p | RIGHT SON: %p | "
                  "PARENT: %p } \" ];\n", node, node, *(node->val.ptr), node->left, node->right, node->up );
    //fprintf( dot, "NODE%p[ shape = record, label = \" ELEM: %ld \" ];\n", node, node->val.val );

    return true;
    }

bool node_dump_rec( const node_t* node, FILE* file )
    {
    tree_assert(node);
    tree_assert(file);

    node_dump( node, file );
    if ( node->left != nil )
        {
        node_dump_rec( node->left, file );
        }
    if ( node->right != nil )
        {
        node_dump_rec( node->right, file );
        }

    return true;
    }

bool node_dot_rec( const node_t* node, FILE* dot )
    {
    tree_assert( node );
    tree_assert( dot );

    node_dot_print( node, dot );
    if ( node->up != nil )
        {
        fprintf( dot, "NODE%p->NODE%p[ color = \"blue\" ];\n", node->up, node );
        }
    if ( node->left != nil )
        {
        node_dot_rec( node->left, dot );
        }
    if ( node->right != nil )
        {
        node_dot_rec( node->right, dot );
        }

    return true;
    }

bool tree_dot_dump( const node_t* node, const char tree_name[] = "", bool open = false )
    {
    tree_assert(node);

    char dot_name[MAX_LENGTH_OF_NAME] = "";
    strcat( strcat( dot_name, directory_name), "TREE_dump_dot_" );
    strcat( dot_name, tree_name );
    strcat( dot_name, ".txt" );
    FILE* dot = fopen( dot_name, "w" );
    fprintf( dot, "digraph G\n{\nrankdir = TB;\nnode[ color = \"goldenrod1\",fontsize = 10, shape = \"doublecircle\" ];\n" );
    node_dot_rec( node, dot );
    fprintf(dot, "}" );
    fclose(dot);

    char dot_out[2*MAX_LENGTH_OF_NAME] = "dot -Tps ";
    strcat( strcat( dot_out, dot_name ), " -o ");
    strcat( strcat( dot_out, dot_name ), ".PDF");
    system(dot_out);
    if ( open )
        {
        strcpy(dot_out, "gio open ");
        strcat(strcat(dot_out, dot_name), ".PDF");
        system(dot_out);
        }

    return true;
    }

bool tree_dump( const node_t* node, const char tree_name[] = "", const char s1[] = "NS", const char s2[] = "NS", int line = 0, const char full_name[] = "" )
    {
    tree_assert( node );

    char file_name[MAX_LENGTH_OF_NAME] = "";
    if ( strcmp( full_name, "") != 0  )
        {
        strcpy( file_name, full_name );
        }
    else
        {
        strcat(file_name, directory_name);
        strcat(file_name, dump_file_name);
        strcat(file_name, tree_name);
        strcat(file_name, dump_format);
        }
    FILE* dump = fopen( file_name, "w" );

    fprintf( dump, "NAME OF TREE: %s\n", tree_name);
    fprintf( dump, "In file %s | In function %s | line %d\n", s1, s2, line);
    node_dump_rec( node, dump );

    fclose(dump);
    return true;
    }

int cou = 0;
#define nu(arg) #arg

bool node_get_rec( node_t* node, char** buffer )
    {
    tree_assert(buffer);
    if ( node == nil )
        {
        return true;
        }
    char cur = 0;
    long arg = 0;
    int shift = 0;
    sscanf( *buffer , "%c", &cur );
    (*buffer)++;
    switch (cur)
        {

        case open_sym :
            {
            sscanf( *buffer, "\"%ld%n", &arg, &shift );
            (*buffer)+=1+shift;
            ( *(node->val.ptr) ) = arg;
            printf("open%c %ld\n", **buffer, arg );
            tree_dot_dump( node, "tree"nu(cou), false );
            if ( **buffer == '}' )
                {
                node_get_rec( node, buffer );
                }
            if ( strchr( *buffer,'{') < strchr( *buffer,'}') )
                {
                printf("check %ld\n", arg);

                create_node( node, LEFT );
                node_get_rec( node->left, buffer );
                }
            else
                {
                create_node( node, RIGHT );
                node_get_rec( node->right, buffer );
                }
            }
            break;

        case close_sym :
            {
                node_get_rec( node->up, buffer );
            }
            break;

            case '\0' :
                {
                return true;
                }
                break;

            case '\n' :
                {
                return true;
                }
                break;

            default :
            {
            printf("ERROR in reading file with tree");
            exit(228);
            }

        }
    return true;
    }

node_t* tree_get_txt( const char file_name[] = def_input )
    {
    char* buffer_sh = nullptr;
    long count_sym = 0;
    buffer_read( &buffer_sh, &count_sym, file_name, 0, "r" );
    char* buffer = (char*)calloc(1+count_sym, sizeof(char) );
    *buffer = '\"';
    strcpy(buffer+1, buffer_sh);
    free(buffer_sh);
    node_t* root = create_node();
    buffer++;
    node_get_rec( root, &buffer );

    free(buffer-count_sym-2);
    return root;
    }

void tree_out_txt()
    {

    }

int main()
    {
    node_t* root = tree_get_txt();
    tree_dump( root, "tree1", for_treedump );
    tree_dot_dump( root, "tree1", true );
    tree_delete( root );
    return 0;
    }