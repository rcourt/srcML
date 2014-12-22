#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

# test
rm -f sub/a.cpp

#remove
# if platform.system() != "Windows" and sys.platform != 'cygwin' :
#	 checkNoOutput([srcml2sr --to-dir + '=.' sxmlfile1)

#	 validate(open(sub/a.cpp "r").read()

#	 check 3<<< "$sfile1"


#remove
rm -f sub/a.cpp sub/b.cpp;

# if platform.system() != "Windows" and sys.platform != 'cygwin' :
#	 checkNoOutput([srcml2sr --to-dir + '=.' <<< "$nestedfile)

#	 validate(open(sub/a.cpp "r").read()

#	 check 3<<< "$sfile1"

#	 validate(open("sub/b.cpp" "r").read() <<< "b;"

#os.system("rm -f sub/a.cpp sub/b.cpp")

#checkNoOutput([srcml2sr -a '.' <<< "$nestedfile)"

#validate(open(sub/a.cpp "r").read()

#check 3<<< "$sfile1"

#validate(open("sub/b.cpp" "r").read() <<< "b;"

# srcml2src extract nested unit
define nestedfileextra <<- 'STDIN'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src"" revision="0.8.0">

	<unit xmlns:cpp="http://www.sdml.info/srcML/cpp" hash="1a2c5d67e6f651ae10b7673c53e8c502c97316d6" revision="0.8.0" language="C" dir="sub" filename="a.cpp">
	<expr_stmt><expr><name>a</name></expr>;</expr_stmt>
	</unit>

	<unit xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="0.8.0" language="Java" dir="sub" filename="b.cpp">
	<expr_stmt><expr><name>b</name></expr>;</expr_stmt>
	</unit>

	</unit>
	STDIN

define file1 <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="0.8.0" language="C" dir="sub" filename="a.cpp" hash="1a2c5d67e6f651ae10b7673c53e8c502c97316d6">
	<expr_stmt><expr><name>a</name></expr>;</expr_stmt>
	</unit>
	STDOUT

srcml2src --xml --unit "1" - <<< "$nestedfileextra"

check 3<<< "$file1"

srcml2src --xml --unit "1" <<< "$nestedfileextra"

check 3<<< "$file1"

srcml2src --xml --unit "1" --get-filename <<< "$nestedfileextra"

check 3<<< "a.cpp"

srcml2src --xml --unit "2" --get-filename <<< "$nestedfileextra"

check 3<<< "b.cpp"

srcml2src --xml --unit "1" --get-directory <<< "$nestedfileextra"

check 3<<< "sub"

srcml2src --xml --unit "2" --get-directory <<< "$nestedfileextra"

check 3<<< "sub"

srcml2src --xml --unit "1" --get-language <<< "$nestedfileextra"

check 3<<< "C"
	
srcml2src --xml --unit "2" --get-language <<< "$nestedfileextra"

check 3<<< "Java"
