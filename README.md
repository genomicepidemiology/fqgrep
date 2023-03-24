# Getting Started #

```
git clone https://bitbucket.org/genomicepidemiology/fqgrep.git
cd fqgrep && make

./fqgrep -v -h
```

# Introduction #
fqgrep greps sequences files against a file of sequence identifiers given through -f/--file.
For practical reasons you might want to add fingerseq to your path, this is usually done with:

```
mv fqgrep ~/bin/
```

# Installation Requirements #
In order to install fingerseq, you need to have a C-compiler and zlib development files installed.
Zlib development files can be installed on unix systems with:
```
sudo apt-get install libz-dev
```

# Acknowledgements #
We thank Mark Adler for the development of gzip.

# Help #
Usage and options are available with the "-h" option. If in doubt, please mail any concerns or 
problems to: *plan@dtu.dk*.

# License #
Copyright (c) 2022, Philip Clausen, Technical University of Denmark
All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
