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
#define _XOPEN_SOURCE 600
#include "filebuff.h"
#include "targets.h"

int segrep(Target *targets, unsigned invert, char **inputfilenames, int se, char *outputfilename);
int intgrep(Target *targets, unsigned invert, char **inputfilenames, int inter, char *outputfilename);
int pegrep(Target *targets, unsigned invert, char **inputfilenames, int pe, char *outputfilename);
int fqgrep(char *targetfilename, unsigned invert, char **inputfilenames, int se, char **intfilenames, int inter, char **pefilenames, int pe, char *outputfilename);
