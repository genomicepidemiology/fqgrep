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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filebuff.h"
#include "fqgrep.h"
#include "pherror.h"
#include "seqparse.h"
#include "targets.h"

int segrep(Target *targets, unsigned invert, char **inputfilenames, int se, char *outputfilename) {
	
	int i;
	unsigned FASTQ;
	char *filename;
	FILE *out;
	FileBuff *inputfile;
	Qseqs *header, *qseq, *qual;
	
	if(!se) {
		return 0;
	}
	
	/* init */
	header = setQseqs(256);
	qseq = setQseqs(1024);
	qual = setQseqs(1024);
	inputfile = setFileBuff(1048576);
	if(*outputfilename == '-' && outputfilename[1] == 0) {
		out = stdout;
	} else {
		out = 0;
	}
	
	for(i = 0; i < se; ++i) {
		filename = inputfilenames[i];
		
		/* determine filetype and open it */
		if((FASTQ = openAndDetermineFQ(inputfile, filename)) & 3) {
			fprintf(stderr, "%s\t%s\n", "# Reading inputfile: ", filename);
		}
		if(!out) {
			filename = smalloc(strlen(outputfilename) + 5);
			if(FASTQ & 1) {
				sprintf(filename, "%s.fq", outputfilename);
			} else {
				sprintf(filename, "%s.fsa", outputfilename);
			}
			out = sfopen(filename, "wb");
			free(filename);
		}
		
		/* parse entries */
		if(FASTQ & 1) {
			while(FileBuffgetFq(inputfile, header, qseq, qual)) {
				//if(0 <= target_grep(targets, (char *)(header->seq))) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)))) & 1) {
					fprintf(out, "@%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
					fprintf(out, "+\n");
					fprintf(out, "%s\n", qual->seq);
				}
			}
		} else if(FASTQ & 2) {
			while(FileBuffgetFsa(inputfile, header, qseq)) {
				//if(0 <= target_grep(targets, (char *)(header->seq))) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)))) & 1) {
					fprintf(out, ">%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
				}
			}
		}
		
		closeFileBuff(inputfile);
	}
	
	/* clean up */
	if(out != stdout) {
		fclose(out);
	}
	destroyQseqs(header);
	destroyQseqs(qseq);
	destroyQseqs(qual);
	destroyFileBuff(inputfile);
	
	return 0;
}

int intgrep(Target *targets, unsigned invert, char **inputfilenames, int inter, char *outputfilename) {
	
	int i;
	unsigned FASTQ;
	char *filename;
	FILE *out;
	FileBuff *inputfile;
	Qseqs *header, *header2, *qseq, *qseq2, *qual, *qual2;
	
	if(!inter) {
		return 0;
	}
	
	/* init */
	header = setQseqs(256);
	header2 = setQseqs(256);
	qseq = setQseqs(1024);
	qseq2 = setQseqs(1024);
	qual = setQseqs(1024);
	qual2 = setQseqs(1024);
	inputfile = setFileBuff(1048576);
	if(*outputfilename == '-' && outputfilename[1] == 0) {
		out = stdout;
	} else {
		out = 0;
	}
	
	for(i = 0; i < inter; ++i) {
		filename = inputfilenames[i];
		
		/* determine filetype and open it */
		if((FASTQ = openAndDetermineFQ(inputfile, filename)) & 3) {
			fprintf(stderr, "%s\t%s\n", "# Reading inputfile: ", filename);
		}
		if(!out) {
			filename = smalloc(strlen(outputfilename) + 9);
			if(FASTQ & 1) {
				sprintf(filename, "%s_int.fq", outputfilename);
			} else {
				sprintf(filename, "%s_int.fsa", outputfilename);
			}
			out = sfopen(filename, "wb");
			free(filename);
		}
		
		/* parse entries */
		if(FASTQ & 1) {
			while(FileBuffgetFq(inputfile, header, qseq, qual) && FileBuffgetFq(inputfile, header2, qseq2, qual2)) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)) || 0 <= target_grep(targets, (char *)(header2->seq)))) & 1) {
					fprintf(out, "@%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
					fprintf(out, "+\n");
					fprintf(out, "%s\n", qual->seq);
					fprintf(out, "@%s\n", header2->seq);
					fprintf(out, "%s\n", qseq2->seq);
					fprintf(out, "+\n");
					fprintf(out, "%s\n", qual2->seq);
				}
			}
		} else if(FASTQ & 2) {
			while(FileBuffgetFsa(inputfile, header, qseq) && FileBuffgetFsa(inputfile, header2, qseq2)) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)) || 0 <= target_grep(targets, (char *)(header2->seq)))) & 1) {
					fprintf(out, ">%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
					fprintf(out, ">%s\n", header2->seq);
					fprintf(out, "%s\n", qseq2->seq);
				}
			}
		}
		
		closeFileBuff(inputfile);
	}
	
	/* clean up */
	if(out != stdout) {
		fclose(out);
	}
	destroyQseqs(header);
	destroyQseqs(header2);
	destroyQseqs(qseq);
	destroyQseqs(qseq2);
	destroyQseqs(qual);
	destroyQseqs(qual2);
	destroyFileBuff(inputfile);
	
	return 0;
}

int pegrep(Target *targets, unsigned invert, char **inputfilenames, int pe, char *outputfilename) {
	
	int i;
	unsigned FASTQ, FASTQ2;
	char *filename;
	FILE *out, *out2;
	FileBuff *inputfile, *inputfile2;
	Qseqs *header, *header2, *qseq, *qseq2, *qual, *qual2;
	
	if(!pe) {
		return 0;
	} else if(pe & 1) {
		fprintf(stderr, "Uneven number of paired end reads.\n");
		return 1;
	}
	
	/* init */
	header = setQseqs(256);
	header2 = setQseqs(256);
	qseq = setQseqs(1024);
	qseq2 = setQseqs(1024);
	qual = setQseqs(1024);
	qual2 = setQseqs(1024);
	inputfile = setFileBuff(1048576);
	inputfile2 = setFileBuff(1048576);
	if(*outputfilename == '-' && outputfilename[1] == 0) {
		out = stdout;
		out2 = stdout;
	} else {
		out = 0;
		out2 = 0;
	}
	
	for(i = 0; i < pe; i += 2) {
		/* determine filetype and open it */
		FASTQ = openAndDetermineFQ(inputfile, inputfilenames[i]);
		FASTQ2 = openAndDetermineFQ(inputfile2, inputfilenames[i+1]);
		if((FASTQ & 3) && (FASTQ2 & 3)) {
			fprintf(stderr, "%s\t%s %s\n", "# Reading inputfiles: ", inputfilenames[i], inputfilenames[i+1]);
		}
		
		if(!out) {
			filename = smalloc(strlen(outputfilename) + 7);
			if(FASTQ & 1) {
				sprintf(filename, "%s_1.fq", outputfilename);
				out = sfopen(filename, "wb");
				sprintf(filename, "%s_2.fq", outputfilename);
				out2 = sfopen(filename, "wb");
			} else {
				sprintf(filename, "%s_1.fsa", outputfilename);
				out = sfopen(filename, "wb");
				sprintf(filename, "%s_2.fsa", outputfilename);
				out2 = sfopen(filename, "wb");
			}
			free(filename);
		}
		
		/* parse entries */
		if((FASTQ & 1) && (FASTQ2 & 1)) {
			while(FileBuffgetFq(inputfile, header, qseq, qual) && FileBuffgetFq(inputfile2, header2, qseq2, qual2)) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)) || 0 <= target_grep(targets, (char *)(header2->seq)))) & 1) {
					fprintf(out, "@%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
					fprintf(out, "+\n");
					fprintf(out, "%s\n", qual->seq);
					fprintf(out2, "@%s\n", header2->seq);
					fprintf(out2, "%s\n", qseq2->seq);
					fprintf(out2, "+\n");
					fprintf(out2, "%s\n", qual2->seq);
				}
			}
		} else if((FASTQ & 2) && (FASTQ2 & 2)) {
			while(FileBuffgetFsa(inputfile, header, qseq) && FileBuffgetFsa(inputfile2, header2, qseq2)) {
				if((invert ^ (0 <= target_grep(targets, (char *)(header->seq)) || 0 <= target_grep(targets, (char *)(header2->seq)))) & 1) {
					fprintf(out, ">%s\n", header->seq);
					fprintf(out, "%s\n", qseq->seq);
					fprintf(out2, ">%s\n", header2->seq);
					fprintf(out2, "%s\n", qseq2->seq);
				}
			}
		} else {
			fprintf(stderr, "%s\t%s %s\n", "# Does not match format: ", inputfilenames[i], inputfilenames[i+1]);
			exit(1);
		}
		
		closeFileBuff(inputfile);
		closeFileBuff(inputfile2);
	}
	
	/* clean up */
	if(out != stdout) {
		fclose(out);
	}
	if(out2 != stdout) {
		fclose(out2);
	}
	destroyQseqs(header);
	destroyQseqs(header2);
	destroyQseqs(qseq);
	destroyQseqs(qseq2);
	destroyQseqs(qual);
	destroyQseqs(qual2);
	destroyFileBuff(inputfile);
	destroyFileBuff(inputfile2);
	
	return 0;
}

int fqgrep(char *targetfilename, unsigned invert, char **inputfilenames, int se, char **intfilenames, int inter, char **pefilenames, int pe, char *outputfilename) {
	
	int error;
	Target *targets;
	
	/* get targets */
	targets = getTargets(targetfilename);
	
	/* get single end matches */
	error = segrep(targets, invert, inputfilenames, se, outputfilename);
	
	/* get interleaved matches */
	error |= intgrep(targets, invert, intfilenames, inter, outputfilename);
	
	/* get paired end matches */
	error |= pegrep(targets, invert, pefilenames, pe, outputfilename);
	
	return error;
}
