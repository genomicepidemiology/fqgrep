/* Philip T.L.C. Clausen Dec 2022 plan@dtu.dk */

/*
 * Copyright (c) 2022, Philip Clausen, Technical University of Denmark
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *		http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filebuff.h"
#include "pherror.h"
#include "qseqs.h"
#include "targets.h"

Target * target_malloc(long unsigned size) {
	
	Target *dest;
	
	dest = smalloc(sizeof(Target));
	dest->n = 0;
	dest->size = size;
	dest->targets = smalloc(size * sizeof(char *));
	
	return dest;
}

Target * target_realloc(Target *src, long unsigned size) {
	
	src->size = size;
	src->targets = realloc(src->targets, size * sizeof(char *));
	if(!src->targets) {
		ERROR();
	}
	
	return src;
}

int entrycmp(char *src1, char *src2) {
	
	static int tolerance = 0; /* here */ /* consider setting it cmd-line */
	int diff, match;
	
	/* set max diff between identifiers */
	if(tolerance < 0 && src1) {
		tolerance = *(int*)(src1);
	}
	
	diff = 0;
	match = 0;
	while(*src1 && *src2) {
		if(*src1 != *src2) {
			if(!match) {
				match = *src1 < *src2 ? -1 : 1;
			}
			if(tolerance < ++diff) {
				return match * diff;
			}
		}
		++src1;
		++src2;
	}
	
	if((*src1 || *src2) && !(isspace(*src1) || isspace(*src2))) {
		if(!match) {
			match = *src1 < *src2 ? -1 : 1;
		}
		while(*src1) {
			if(tolerance < ++diff) {
				return match * diff;
			}
			++src1;
		}
		while(*src2) {
			if(tolerance < ++diff) {
				return match * diff;
			}
			++src2;
		}
	}
	
	if(diff <= tolerance) {
		diff = 0;
	}
	
	return match * diff;
}

int target_grep(Target *src, char *entry) {
	
	int downlim, uplim, index, match;
	char **targets;
	
	if(src->n == 0) {
		return -1;
	}
	
	/* init */
	targets = src->targets;
	
	downlim = 0;
	uplim = src->n - 1;
	while(downlim <= uplim) {
		index = (downlim + uplim) >> 1;
		match = entrycmp(entry, targets[index]);
		if(match < 0) { /* lower */
			uplim = index - 1;
		} else if(0 < match) { /* upper */
			downlim = index + 1;
		} else { /* match */
			return index;
		}
	}
	
	return -1;
}

int target_duppush(Target *dest, Qseqs *target_entry) {
	
	int match;
	char *entry;
	
	/* cpy entry */
	entry = smalloc(target_entry->len + 1);
	memcpy(entry, target_entry->seq, target_entry->len + 1);
	
	/* realloc */
	if(dest->n == dest->size) {
		dest = target_realloc(dest, dest->size << 1);
	}
	
	/* check if entry is already added */
	match = dest->n ? entrycmp(dest->targets[dest->n - 1], entry) : -1;
	if(match) {
		/* add entry */
		dest->targets[dest->n++] = entry;
	} else {
		free(entry);
	}
	
	return 0 < match;
}

int target_dedup(Target *src) {
	
	int n;
	char **org, **dest;
	
	if(!src->n) {
		return 0;
	}
	
	/* parse targets */
	org = src->targets;
	dest = src->targets;
	n = src->n;
	while(--n) {
		if(entrycmp(*dest, *++org)) { /* adjust array */
			*++dest = *org;
		} else { /* duplcate */
			/* remove duplicate */
			src->n--;
			free(*org);
		}
	}
	
	return src->n;
}

int getTarget(FileBuff *src, Qseqs *entry) {
	
	unsigned char *buff, *seq;
	int size, avail;
	int (*buffFileBuff)(FileBuff *);
	
	/* init */
	avail = src->bytes;
	buff = src->next;
	buffFileBuff = src->buffFileBuff;
	if(avail == 0) {
		if((avail = buffFileBuff(src)) == 0) {
			return 0;
		}
		buff = src->buffer;
	}
	
	/* get entry */
	seq = entry->seq;
	size = entry->size;
	while((*seq++ = *buff++) != '\n') {
		if(--avail == 0) {
			if((avail = buffFileBuff(src)) == 0) {
				return 0;
			}
			buff = src->buffer;
		}
		if(--size == 0) {
			size = entry->size;
			entry->size <<= 1;
			entry->seq = realloc(entry->seq, entry->size);
			if(!entry->seq) {
				ERROR();
			}
			seq = entry->seq + size;
		}
	}
	
	/* chomp entry */
	while(isspace(*--seq)) {
		++size;
	}
	*++seq = 0;
	entry->len = entry->size - size + 1;
	
	src->bytes = --avail;
	src->next = buff;
	
	return 1;
}

static int stringcmp(const void *str1, const void *str2) {
	return entrycmp(*(char **)(str1), *(char **)(str2));
}

Target * getTargets(char *targetfilename) {
	
	int sorted;
	FileBuff *inputfile;
	Qseqs *entry;
	Target *dest;
	
	/* init */
	dest = target_malloc(1024);
	inputfile = setFileBuff(1048576);
	entry = setQseqs(256);
	
	/* open target file */
	openAndDetermine(inputfile, targetfilename);
	
	/* parse target file */
	sorted = 0;
	while(getTarget(inputfile, entry)) {
		sorted |= target_duppush(dest, entry);
	}
	
	/* sort list */
	if(sorted) {
		qsort(dest->targets, dest->n, sizeof(char *), stringcmp);
		target_dedup(dest);
	}
	
	/* clean up */
	closeFileBuff(inputfile);
	destroyFileBuff(inputfile);
	destroyQseqs(entry);
	dest = target_realloc(dest, dest->n);
	
	return dest;
}
