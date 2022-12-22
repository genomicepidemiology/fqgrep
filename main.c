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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmdline.h"
#include "fqgrep.h"
#include "version.h"
#define missArg(opt) fprintf(stderr, "Missing argument at %s.\n", opt); exit(1);
#define invaArg(opt) fprintf(stderr, "Invalid value parsed at %s.\n", opt); exit(1);

static int helpMessage(FILE *out) {
	
	fprintf(out, "#fqgrep greps sequences entries from fasta and fastq files from a list of sorted identifiers.\n");
	fprintf(out, "#   %-24s\t%-32s\t%s\n", "Options are:", "Desc:", "Default:");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'f', "file", "Newline separated file with target identifiers.", "");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'i', "input", "Input file(s) single end.", "stdin");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'I', "interleaved", "Input file(s) interleaved.", "");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'p', "paired", "Input file(s) paired end.", "");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'o', "output", "Output file(s).", "stdout");
	fprintf(out, "#    -%c, --%-17s\t%-32s\t%s\n", 'v', "version", "Version.", "");
	fprintf(out, "#    -%c, --%-16s\t%-32s\t%s\n", 'h', "help", "Shows this helpmessage.", "");
	
	return out == stderr;
}

int main(int argc, char *argv[]) {
	
	const char *stdstream = "-";
	int args, len, offset, se, pe, inter;
	char **Arg, *arg, *outputfilename, *targetfilename, opt;
	char **inputfilenames, **intfilenames, **pefilenames;
	
	/* set defaults */
	outputfilename = (char *)(stdstream);
	targetfilename = 0;
	se = 0;
	inputfilenames = 0;
	inter = 0;
	intfilenames = 0;
	pe = 0;
	pefilenames = 0;
	
	/* parse cmd-line */
	args = argc - 1;
	Arg = argv;
	if(args && **++Arg == '-') {
		len = 1;
		--Arg;
	} else {
		len = 0;
	}
	while(args && len) {
		arg = *++Arg;
		if(*arg++ == '-') {
			if(*arg == '-') {
				/* check if argument is included */
				len = getOptArg(++arg);
				offset = 2 + (arg[len] ? 1 : 0);
				
				/* long option */
				if(*arg == 0) {
					/* terminate cmd-line */
					++Arg;
				} else if(len == 5 && strncmp(arg, "input", len) == 0) {
					inputfilenames = getArgListDie(&Arg, &args, len + offset, "input");
					se = getArgListLen(&Arg, &args);
				} else if(len == 11 && strncmp(arg, "interleaved", len) == 0) {
					intfilenames = getArgListDie(&Arg, &args, len + offset, "interleaved");
					inter = getArgListLen(&Arg, &args);
				} else if(len == 6 && strncmp(arg, "paired", len) == 0) {
					pefilenames = getArgListDie(&Arg, &args, len + offset, "paired");
					pe = getArgListLen(&Arg, &args);
				} else if(len == 6 && strncmp(arg, "output", len) == 0) {
					outputfilename = getArgDie(&Arg, &args, len + offset, "output");
				} else if(len == 4 && strncmp(arg, "file", len) == 0) {
					targetfilename = getArgDie(&Arg, &args, len + offset, "file");
				} else if(len == 7 && strncmp(arg, "version", len) == 0) {
					fprintf(stdout, "fqgrep-%s\n", FQGREP_VERSION);
				} else if(len == 4 && strncmp(arg, "help", len) == 0) {
					return helpMessage(stdout);
				} else {
					unknArg(arg - 2);
				}
			} else {
				/* multiple option */
				len = 1;
				opt = *arg;
				while(opt && (opt = *arg++)) {
					++len;
					if(opt == 'i') {
						inputfilenames = getArgListDie(&Arg, &args, len, "i");
						se = getArgListLen(&Arg, &args);
						opt = 0;
					} else if(opt == 'I') {
						intfilenames = getArgListDie(&Arg, &args, len, "I");
						inter = getArgListLen(&Arg, &args);
						opt = 0;
					} else if(opt == 'p') {
						pefilenames = getArgListDie(&Arg, &args, len, "p");
						pe = getArgListLen(&Arg, &args);
						opt = 0;
					} else if(opt == 'o') {
						outputfilename = getArgDie(&Arg, &args, len, "o");
						opt = 0;
					} else if(opt == 'f') {
						targetfilename = getArgDie(&Arg, &args, len, "f");
						opt = 0;
					} else if(opt == 'v') {
						fprintf(stdout, "fqgrep-%s\n", FQGREP_VERSION);
					} else if(opt == 'h') {
						return helpMessage(stdout);
					} else {
						*arg = 0;
						unknArg(arg - 1);
					}
				}
			}
		} else {
			/* terminate cmd-line */
			--arg;
			++args;
			len = 0;
		}
		--args;
	}
	
	/* non-options */
	if(args) {
		inputfilenames = Arg;
		se = args;
	}
	
	/* check input */
	if((se + pe + inter) == 0) {
		fprintf(stderr, "Missing input.\n");
		return helpMessage(stderr);
	} else if(!targetfilename) {
		fprintf(stderr, "Missing entry target(s).\n");
		return helpMessage(stderr);
	}
	
	/* fqgrep */
	return fqgrep(targetfilename, inputfilenames, se, intfilenames, inter, pefilenames, pe, outputfilename);
}
