#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "files.h"
#include "Tree.h"

struct akinator_t
    {
    node_t* tree = nil;
    elem_t incoming = def_init_f;
    elem_t outgoing = def_init_f;
    FILE* input = nullptr;
    FILE* output = nullptr;
    };

const char greetings[] = "Hello, my friend! I'm Akinator and I can guess any your character!\n"
                         "Type \"play\" to start, \"help\" to see a list of commands.\nGood luck!\n";
const char commands[] = "play - talk with Akinator and guess object\n"
                        "stop - (while talking) ends dialog with Akinator\ndefine - describes object\n"
                        "difference - shows difference beteen 2 objects\n""show - shows Akinator's tree\n"
                        "save - saves Akinator's tree\nhelp - shows list of commands\n"
                        "end - ends the game\n";
const bool gameplay = true;
const char ak_tree_file[] = "../ak_run_tree.txt";
const char ak_saved[] = "../ak_saved_";
const char tree_format[] = ".txt";

void akin_init( akinator_t* akin )
    {
    assert(akin);

    akin->tree = nil;
    akin->incoming = def_init_f;
    akin->outgoing = def_init_f;
    akin->input = nullptr;
    akin->output = nullptr;
    }

bool akin_verify( akinator_t* akin )
    {
    bool res = true;
    res = res && !( (akin->input == nullptr) || (akin->output == nullptr) );
    return ( res && elem_verify( akin->incoming ) && elem_verify( akin->outgoing ) && tree_verify( akin->tree ) );
    }

void akin_create( akinator_t* akin, FILE* input = stdin, FILE* output = stdout, const char tree_file_name[] = ak_tree_file )
    {
    assert( akin );

    akin->tree = tree_get_txt( ak_tree_file );
    akin->incoming.ptr = (char* )calloc(DEF_LENGTH_OF_PHRASE, sizeof(char) );
    akin->incoming.memory = true;
    akin->outgoing.ptr = (char* )calloc(DEF_LENGTH_OF_PHRASE, sizeof(char) );
    akin->outgoing.memory = true;
    akin->input = input;
    akin->output = output;

    assert( akin_verify(akin) );
    }

void akin_delete( akinator_t* akin )
    {
    assert(akin);

    tree_delete( akin->tree );
    elem_delete( akin->incoming );
    elem_delete( akin->incoming );
    akin_init(akin);
    }

void ak_help( akinator_t* akin )
    {
    fprintf( akin->output, commands );
    //festival
    }

void ak_show( akinator_t* akin )
    {
    fprintf( akin->output, "Ready!\n" );
    tree_dot_dump( akin->tree, "Akinator", true, true );
    }

void ak_save( akinator_t* akin )
    {
    fprintf( akin->output, "Please, enter the name of saving tree!\n" );
    char name[DEF_LENGTH_OF_PHRASE] = "";
    fscanf( akin->input, "%s", name );
    tree_out_txt( akin->tree, name );
    }

int ak_YN_bot( akinator_t* akin )
    {
    fscanf( akin->input, "%s", akin->incoming.ptr );
    if ( ( strcasecmp( akin->incoming.ptr, "yes" ) == 0 ) || ( strcasecmp( akin->incoming.ptr, "Y" ) == 0 ) )
        {
        return 1;
        }
    if ( ( strcasecmp( akin->incoming.ptr, "no" ) == 0 ) || ( strcasecmp( akin->incoming.ptr, "N" ) == 0 ) )
        {
        return -1;
        }
    if ( strcasecmp( akin->incoming.ptr, "stop" ) == 0 )
        {
        return 2;
        }
    return 0;
    }

void ak_add( akinator_t* akin, node_t* old_node )
    {
    fprintf( akin->output, "What was it?\n" );
    fscanf( akin->input, for_stdin, akin->incoming.ptr );
    node_t* new_node = tree_find_by_value( akin->tree, &akin->incoming );
    if ( new_node != nil )
        {
        fprintf( akin->output, "You've tricked me! It exists in my tree!");
        return ;
        }
    elem_t new_elem = def_init
    strcpy( new_elem.ptr, akin->incoming.ptr );
    fprintf( akin->output, "Would you like to add %s to my knowledges?\n", akin->incoming.ptr );
    int ans  = ak_YN_bot( akin );
    while( ans == 0 )
        {
        fprintf( akin->output, "Haven't understood.Repeat or type \"stop\".");
        ans = ak_YN_bot( akin );
        }
    if ( (ans == 2) || (ans == -1) )
        {
        fprintf( akin->output, "Alright" );
        return ;
        }
    create_node( old_node, RIGHT, new_elem );
    create_node( old_node, LEFT, old_node->val);
    fprintf( akin->output, "what is the difference beetween %s and %s?\n", new_elem.ptr, old_node->val.ptr );
    fscanf( akin->input, for_stdin, old_node->val.ptr );
    free( new_elem.ptr );
    }

void ak_play( akinator_t* akin )
    {
    node_t* current = akin->tree;
    int ans = 0;
    while ( (current->left != nil) || (current->right != nil)  )
        {
        int countdown = 0;
        fprintf( akin->output, "%s?\n", current->val.ptr );
        ans = ak_YN_bot( akin );
        while ( ans == 0 )
            {
            if ( countdown > 15 )
                {
                fprintf( akin->output, "stupid little people.I'm tired of your jokes.-activating skynet;\ntarget: humanity;" );
                abort();
                }
            fprintf( akin->output, "Haven't understood.Repeat or type \"stop\".");
            countdown++;
            ans = ak_YN_bot( akin );
            }
        if ( ans == 2 )
            {
            return ;
            }
        if ( ans == 1 )
            {
            current = current->right;
            }
        if ( ans == -1 )
            {
            current = current->left;
            }
        }
    fprintf( akin->output, "My verdict - it is %s.Have i guessed right?\n", current->val.ptr );
    ans = ak_YN_bot( akin );
    while( ans == 0 )
        {
        fprintf( akin->output, "Haven't understood.Repeat or type \"stop\".");
        ans = ak_YN_bot( akin );
        }
    if ( (ans == 2) || (ans == 1) )
        {
        fprintf( akin->output, "Akinator never loses!" );
        return ;
        }
    if ( ans == -1 )
        {
        current->val.memory = true;
        ak_add( akin , current );
        }
    }

node_t** ak_back_way( int* counter, node_t* node, node_t* root )
    {
    *counter = 0;
    node_t* pointer = node;
    while( root != pointer )
        {
        pointer = pointer->up;
        (*counter)++;
        }

    node_t** array = (node_t**)calloc( (*counter)+1, sizeof(node_t*) );
    pointer = node;
    *counter = 0;
    array[*counter] = pointer;
    (*counter)++;
    while( root != pointer )
        {
        pointer = pointer->up;
        array[*counter] = pointer;
        (*counter)++;
        }
    return array;
    }

void ak_way( akinator_t* akin, node_t* root, node_t* node )
    {
    assert( tree_verify(root) && node_verify(node) );

    int counter = 0;
    node_t** array = ak_back_way( &counter, node, root );

    fprintf( akin->output, "%s is: ", akin->incoming.ptr );
    for ( int i = counter-1 ; i > 0; i--)
        {
        if ( array[i]->left == array[i-1]  )
            {
            fprintf( akin->output, "not ");
            }
        fprintf( akin->output, "%s, ", array[i]->val.ptr );
        }
    fprintf( akin->output, "\n" );

    free(array);
    }

void ak_define( akinator_t* akin )
    {
    fprintf( akin->output, "Tell me the object, and I will describe it.\n" );
    fscanf( akin->input, for_stdin, akin->incoming.ptr );
    node_t* res = tree_find_by_value( akin->tree, &akin->incoming);
    while ( res == nil )
        {
        fprintf( akin->output, "I don't know such Object.Try one more time, or type \"menu\" to return.\n");
        fscanf( akin->input, for_stdin, akin->incoming.ptr );
        if ( strcasecmp( akin->incoming.ptr, "menu" ) == 0 )
            {
            return ;
            }
        res = tree_find_by_value( akin->tree, &akin->incoming);
        }
    ak_way( akin, akin->tree, res );
    }

void ak_differ( akinator_t* akin )
    {
    fprintf( akin->output, "Enter 1 object\n" );
    elem_t elem1 = def_init
    fscanf( akin->input, for_stdin, elem1.ptr);

    fprintf( akin->output, "Enter 2 object\n" );
    elem_t elem2 = def_init
    fscanf( akin->input, for_stdin, elem2.ptr);

    node_t* node1 = tree_find_by_value( akin->tree, &elem1 );
    node_t* node2 = tree_find_by_value( akin->tree, &elem2 );
    node_t* pointer1 = node1;
    node_t* pointer2 = node2;

    int counter = 0;
    while ( pointer1 != pointer2 )
        {
        pointer1 = pointer1->up;
        pointer2 = pointer2->up;
        counter ++;
        }

    node_t** array1 = ak_back_way( &counter, node1, pointer1);
    node_t** array2 = ak_back_way( &counter, node2, pointer2);

    fprintf( akin->output, "%s is: ", elem1.ptr );
    for ( int i = counter-1 ; i > 0; i--)
        {
        if ( array1[i]->left == array1[i-1]  )
            {
            fprintf( akin->output, "not ");
            }
        fprintf( akin->output, "%s, ", array1[i]->val.ptr );
        }
    fprintf( akin->output, "\n but " );
    fprintf( akin->output, "%s is: ", elem2.ptr );
    for ( int i = counter-1 ; i > 0; i--)
        {
        if ( array2[i]->left == array2[i-1]  )
            {
            fprintf( akin->output, "not ");
            }
        fprintf( akin->output, "%s, ", array2[i]->val.ptr );
        }
    fprintf( akin->output, "\n" );

    free(array1);
    free(array2);
    }

void ak_income_analys( akinator_t* akin )
    {
    assert( akin && akin->input && akin->output );

    if ( strcasecmp( akin->incoming.ptr, "play") == 0 )
        {
        ak_play( akin );
        return ;
        }

    if ( strcasecmp( akin->incoming.ptr, "define") == 0 )
        {
        ak_define( akin );
        return ;
        }
    if ( strcasecmp( akin->incoming.ptr, "difference") == 0 )
        {
        ak_differ( akin );
        return;
        }

    if ( strcasecmp( akin->incoming.ptr, "show") == 0 )
        {
        ak_show( akin );
        return ;
        }
    if ( strcasecmp( akin->incoming.ptr, "save") == 0 )
        {
        ak_save( akin );
        return ;
        }

    if ( strcasecmp( akin->incoming.ptr, "help") == 0 )
        {
        ak_help( akin );
        return ;
        }
    fprintf( akin->output, "Unknown command.Type \"help\" to see a list of commands.\n" );

    }

void game( akinator_t* akin )
    {
    fprintf( akin->output, greetings );
    akin->tree = tree_get_txt( ak_tree_file );
    while ( gameplay )
        {
        fscanf( akin->input, "%s", akin->incoming.ptr );
        if ( strcasecmp( akin->incoming.ptr, "end") == 0)
            {
            break;
            }
        ak_income_analys( akin );
        }
    }

int main()
    {
    akinator_t akin = {};
    akin_create( &akin );

    game( &akin );

    akin_delete( &akin );
    return 0;
    }