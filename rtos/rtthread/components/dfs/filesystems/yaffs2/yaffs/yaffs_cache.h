/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __YAFFS_CACHE_H__
#define __YAFFS_CACHE_H__

#include "yaffs_guts.h"


/* Does the object have a dirty cache ? */
int yaffs_obj_cache_dirty(struct yaffs_obj *obj);

/* Flush the cache associated with a file, either discarding or keeping */
void yaffs_flush_file_cache(struct yaffs_obj *obj, int discard);

/* Flush everything in the cache, either discarding or keeping */
void yaffs_flush_whole_cache(struct yaffs_dev *dev, int discard);

/* Grab a cache item during read or write. */
struct yaffs_cache *yaffs_grab_chunk_cache(struct yaffs_dev *dev);

/* Find a cached chunk */
struct yaffs_cache *yaffs_find_chunk_cache(const struct yaffs_obj *obj,
        int chunk_id);

/* Mark the chunk for the least recently used algorithym */
void yaffs_use_cache(struct yaffs_dev *dev, struct yaffs_cache *cache,
                     int is_write);


/* Invalidate a single cache page. The cache no longer holds valid data. */
void yaffs_invalidate_chunk_cache(struct yaffs_obj *object,
                                  int chunk_id);

/* Invalidate all the cache pages associated with this object
 * Do this whenever ther file is deleted or resized.
 */
void yaffs_invalidate_file_cache(struct yaffs_obj *in);


int yaffs_count_dirty_caches(struct yaffs_dev *dev);

/* Init/deinit. */
int yaffs_cache_init(struct yaffs_dev *dev);
void yaffs_cache_deinit(struct yaffs_dev *dev);

#endif
