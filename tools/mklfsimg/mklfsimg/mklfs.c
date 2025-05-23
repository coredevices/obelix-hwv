/*
 * Copyright (C) 2015 - 2020, IBEROXARXA SERVICIOS INTEGRALES, S.L.
 * Copyright (C) 2015 - 2020, Jaume Olivé Petrus (jolive@whitecatboard.org)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *     * The WHITECAT logotype cannot be changed, you can remove it, but you
 *       cannot change it in any way. The WHITECAT logotype is:
 *
 *          /\       /\
 *         /  \_____/  \
 *        /_____________\
 *        W H I T E C A T
 *
 *     * Redistributions in binary form must retain all copyright notices printed
 *       to any local or remote output device. This include any reference to
 *       Lua RTOS, whitecatboard.org, Lua, and other copyright notices that may
 *       appear in the future.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Lua RTOS, a tool for make a LFS file system image
 *
 */

#include "lfs.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <windows.h>
#include <errno.h>
#include <limits.h>
//#include <dirent.h>
#include "getopt.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifndef PATH_MAX
#define PATH_MAX (256)
#endif

#define OUTPUT_REMAINING_FILE_SIZE()   do \
                                       {  \
                                           fprintf(stdout, "Not packed: %.2f KB\r\n", (float)remaining_size / 1024); \
                                           fprintf(stdout, "Disk Total: %d KB\r\n", lfs.cfg->block_count * lfs.cfg->block_size / 1024); \
                                       } while(0)  

static struct lfs_config cfg;
static lfs_t lfs;
static uint8_t *data;
static int remaining_size;


static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    memcpy(buffer, data + (block * c->block_size) + off, size);
	//printf("rd:%d-%d,%d\n", block, off / c->read_size, size);
    return 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
	memcpy(data + (block * c->block_size) + off, buffer, size);
	//printf("wr:%d-%d,%d\n", block, off / c->prog_size, size);
    return 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block) {
    memset(data + (block * c->block_size), 0xff, c->block_size);
	//printf("erase:%d\n", block);
    return 0;
}

static int lfs_sync(const struct lfs_config *c) {
	return 0;
}

static void create_dir(char *src) {
    char *path;
    int ret;

    path = strchr(src, '/');
    if (path) {
        fprintf(stdout, "%s\r\n", path);

		if ((ret = lfs_mkdir(&lfs, path)) < 0) {
			fprintf(stderr,"can't create directory %s: error=%d\r\n", path, ret);
			OUTPUT_REMAINING_FILE_SIZE();
			exit(1);
		}
	}
}

static void create_file(char *src) {
    char *path;
    int ret;
	struct stat s;
	
    path = strchr(src, '/');
    if (path) {
        fprintf(stdout, "%s\r\n", path);

        // Open source file
        FILE *srcf = fopen(src,"rb");
        if (!srcf) {
            fprintf(stderr,"can't open source file %s: errno=%d (%s)\r\n", src, errno, strerror(errno));
            exit(1);
        }

        // Open destination file
        lfs_file_t dstf;
        if ((ret = lfs_file_open(&lfs, &dstf, path, LFS_O_WRONLY | LFS_O_CREAT)) < 0) {
            fprintf(stderr,"can't open destination file %s: error=%d\r\n", path, ret);
			OUTPUT_REMAINING_FILE_SIZE();
            exit(1);
        }

		char c = fgetc(srcf);
		while (!feof(srcf)) {
			ret = lfs_file_write(&lfs, &dstf, &c, 1);
			if (ret < 0) {
				fprintf(stderr,"can't write to destination file %s: error=%d\r\n", path, ret);
				OUTPUT_REMAINING_FILE_SIZE();
				exit(1);
			}
			c = fgetc(srcf);
		}

        // Close destination file
		ret = lfs_file_close(&lfs, &dstf);
		if (ret < 0) {
			fprintf(stderr,"can't close destination file %s: error=%d\r\n", path, ret);
			exit(1);
		}

        // Close source file
        fclose(srcf);

		stat(src, &s);
		if (remaining_size >= s.st_size)
		{
			remaining_size -= s.st_size;
		}
    }
}

static void compact(char *src) {
    //DIR *dir;
	HANDLE dir;
    //struct dirent *ent;
    char curr_path[PATH_MAX];
	char *search_files;
	static WIN32_FIND_DATA find_data;

	search_files = malloc(strlen(src) + strlen("/*") + 1);
	assert(search_files);
	search_files[0] = 0;
	strcat(search_files, src);
	strcat(search_files, "/*");


    //dir = opendir(src);
	dir = FindFirstFile(search_files, &find_data);
    if (INVALID_HANDLE_VALUE != dir) {
        //while ((ent = readdir(dir))) {
		while (1) {
            // Skip . and .. directories
            if ((strcmp(find_data.cFileName,".") != 0) && (strcmp(find_data.cFileName,"..") != 0)) {
                // Update the current path
                strcpy(curr_path, src);
                strcat(curr_path, "/");
                strcat(curr_path, find_data.cFileName);

                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    create_dir(curr_path);
                    compact(curr_path);
                } else {// if (find_data.dwFileAttributes == DT_REG) {
                    create_file(curr_path);
                }
            }

			if (!FindNextFile(dir, &find_data)) {
				break;
			}
        }

		FindClose(dir);
    }
}

int calc_dir_size(char *src)
{
	//DIR *dir;
	HANDLE dir;
	//struct dirent *ent;
	char curr_path[PATH_MAX];
	char *search_files;
	static WIN32_FIND_DATA find_data;
	int total_size;
	struct stat s;

	search_files = malloc(strlen(src) + strlen("/*") + 1);
	assert(search_files);
	search_files[0] = 0;
	strcat(search_files, src);
	strcat(search_files, "/*");

	total_size = 0;
	
	dir = FindFirstFile(search_files, &find_data);
	if (INVALID_HANDLE_VALUE != dir) {
		while (1) {
			// Skip . and .. directories
			if ((strcmp(find_data.cFileName, ".") != 0) && (strcmp(find_data.cFileName, "..") != 0)) {
				// Update the current path
				strcpy(curr_path, src);
				strcat(curr_path, "/");
				strcat(curr_path, find_data.cFileName);

				if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					total_size += calc_dir_size(curr_path);
				}
				else {// if (find_data.dwFileAttributes == DT_REG) {
					if (0 == stat(curr_path, &s))
					{
						total_size += s.st_size;
					}
				}
			}

			if (!FindNextFile(dir, &find_data)) {
				break;
			}
		}

		FindClose(dir);
	}

	return total_size;
}



void usage() {
	fprintf(stdout, "usage: mklfs -c <pack-dir> -b <block-size> -r <read-size> -p <prog-size> -s <filesystem-size> -i <image-file-path>\r\n");
}

static int is_number(const char *s) {
	const char *c = s;

	while (*c) {
		if ((*c < '0') || (*c > '9')) {
			return 0;
		}
		c++;
	}

	return 1;
}

static int is_hex(const char *s) {
	const char *c = s;

	if (*c++ != '0') {
		return 0;
	}

	if (*c++ != 'x') {
		return 0;
	}

	while (*c) {
		if (((*c < '0') || (*c > '9')) && ((*c < 'A') || (*c > 'F')) && ((*c < 'a') || (*c > 'f'))) {
			return 0;
		}
		c++;
	}

	return 1;
}

static int to_int(const char *s) {
	if (is_number(s)) {
		return atoi(s);
	} else if (is_hex(s)) {
		return (int)strtol(s, NULL, 16);
	}

	return -1;
}

#ifdef LFS_THREADSAFE
// Lock the underlying block device. Negative error codes
// are propogated to the user.
int lfs_lock(const struct lfs_config *c)
{
	return 0;
}

// Unlock the underlying block device. Negative error codes
// are propogated to the user.
int lfs_unlock(const struct lfs_config *c)
{
	return 0;
}
#endif /* LFS_THREADSAFE */

int main(int argc, char **argv) {
    char *src = NULL;   // Source directory
    char *dst = NULL;   // Destination image
    int c;              // Current option
    int block_size = 0; // Block size
    int read_size = 0;  // Read size
    int prog_size = 0;  // Prog size
    int fs_size = 0;    // File system size
    int err;
	struct getopt_state state;
	lfs_ssize_t used_size;
	int total_size;

	getopt_init(&state);
	while ((c = getopt(&state, argc, argv, "c:i:b:p:r:s:")) != -1) {
		switch (c) {
		case 'c':
			src = state.optarg;
			break;

		case 'i':
			dst = state.optarg;
			break;

		case 'b':
			block_size = to_int(state.optarg);
			break;

		case 'p':
			prog_size = to_int(state.optarg);
			break;

		case 'r':
			read_size = to_int(state.optarg);
			break;

		case 's':
			fs_size = to_int(state.optarg);
			break;
		}
	}

    if ((src == NULL) || (dst == NULL) || (block_size <= 0) || (prog_size <= 0) ||
        (read_size <= 0) || (fs_size <= 0)) {
    		usage();
        exit(1);
    }

    // Mount the file system
    cfg.read  = lfs_read;
    cfg.prog  = lfs_prog;
    cfg.erase = lfs_erase;
    cfg.sync  = lfs_sync;
#ifdef LFS_THREADSAFE
	cfg.lock = lfs_lock;
	cfg.unlock = lfs_unlock;
#endif

    cfg.block_size  = block_size;
    cfg.read_size   = read_size;
    cfg.prog_size   = prog_size;
    cfg.block_count = fs_size / cfg.block_size;
    cfg.lookahead_size = 32 * ((cfg.block_count + 31) / 32);;
	if (cfg.lookahead_size > 128)
	{
		cfg.lookahead_size = 128;
	}
    //cfg.lookahead_size   = read_size;
    cfg.context     = NULL;
	cfg.cache_size = prog_size;
	cfg.block_cycles = -1;

	data = calloc(1, fs_size);
	if (!data) {
		fprintf(stderr, "no memory for mount\r\n");
		return -1;
	}
	memset(data, 0xFF, fs_size);

	err = lfs_format(&lfs, &cfg);
	if (err < 0) {
		fprintf(stderr, "format error: error=%d\r\n", err);
		return -1;
	}

	err = lfs_mount(&lfs, &cfg);
	if (err < 0) {
		fprintf(stderr, "mount error: error=%d\r\n", err);
		return -1;
	}

	total_size = calc_dir_size(src);
	remaining_size = total_size;

	fprintf(stdout, "Files Total: %.2f KB\r\n", (float)total_size / 1024);

	compact(src);

	FILE *img = fopen(dst, "wb+");

	if (!img) {
		fprintf(stderr, "can't create image file: errno=%d (%s)\r\n", errno, strerror(errno));
     	return -1;
	}

	fwrite(data, 1, fs_size, img);

	fclose(img);

	used_size = lfs_fs_size(&lfs);
	fprintf(stdout, "Disk Total: %d KB, Free: %d KB\r\n", lfs.cfg->block_count * lfs.cfg->block_size / 1024, (lfs.cfg->block_count - used_size) * lfs.cfg->block_size / 1024);

	return 0;
}
