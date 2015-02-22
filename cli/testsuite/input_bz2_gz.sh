#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

# test on compressed files with .gz extension
define src <<- 'STDOUT'

	a;
	STDOUT

define foutput <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="0.8.0" language="C++" filename="archive/a.cpp.bz2.gz">
	<expr_stmt><expr><name>a</name></expr>;</expr_stmt>
	</unit>
	STDOUT

define output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="0.8.0" language="C++">
	<expr_stmt><expr><name>a</name></expr>;</expr_stmt>
	</unit>
	STDOUT

createfile archive/a.cpp "$src"
bzip2 -c archive/a.cpp > archive/a.cpp.bz2
gzip -c archive/a.cpp.bz2 > archive/a.cpp.bz2.gz

src2srcml archive/a.cpp.bz2.gz -o archive/a.cpp.xml
check archive/a.cpp.xml 3<<< "$foutput"

srcml archive/a.cpp.bz2.gz
check 3<<< "$foutput"

srcml -l C++ < archive/a.cpp.bz2.gz
check 3<<< "$output"

srcml -l C++ -o archive/a.cpp.xml < archive/a.cpp.bz2.gz
check archive/a.cpp.xml 3<<< "$output"