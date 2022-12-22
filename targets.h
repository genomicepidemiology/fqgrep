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
#include "qseqs.h"

#ifndef TARGETS
typedef struct target Target;
struct target {
	int n;
	int size;
	char **targets;
};
#define TARGETS 1
#endif

Target * target_malloc(long unsigned size);
Target * target_realloc(Target *src, long unsigned size);
int entrycmp(char *src1, char *src2);
int target_grep(Target *src, char *entry);
int target_duppush(Target *dest, Qseqs *target_entry);
int target_dedup(Target *src);
int getTarget(FileBuff *src, Qseqs *entry);
Target * getTargets(char *targetfilename);
