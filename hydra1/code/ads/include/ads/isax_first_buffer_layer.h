//
//  first_buffer_layer.h
//  isaxlib
//
//  Created by Kostas Zoumpatianos on 3/20/12.
//  Copyright 2012 University of Trento. All rights reserved.
//

#ifndef isaxlib_first_buffer_layer_h
#define isaxlib_first_buffer_layer_h
#include "../../config.h"
#include "../../globals.h"
#include "isax_node.h"
#include "isax_index.h"

typedef struct fbl_soft_buffer {
    isax_node *node;
    sax_type ** sax_records;
    file_position_type ** pos_records;
    int max_buffer_size;
    int buffer_size;
    int initialized;    
} fbl_soft_buffer;

typedef struct first_buffer_layer {
    int number_of_buffers;
    int initial_buffer_size;
    int max_total_size;
    int current_record_index;
    char *current_record;
    char *hard_buffer;
    fbl_soft_buffer *soft_buffers;
    
} first_buffer_layer;

first_buffer_layer * initialize_fbl(int initial_buffer_size, int max_fbl_size, 
                                    unsigned long long max_total_size, isax_index *index);
void destroy_fbl(first_buffer_layer *fbl);
isax_node * insert_to_fbl(first_buffer_layer *fbl, sax_type *sax,
                          file_position_type * pos, root_mask_type mask, 
                          isax_index *index);
enum response flush_fbl(first_buffer_layer *fbl, isax_index *index);
#endif
