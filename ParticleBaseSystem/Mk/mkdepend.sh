#!/bin/sh
exit
echo ${1%.cod}.hex: \\
/cygdrive/c/Program\ Files\ \(x86\)/gputils/bin/gpvc -d $1|awk ' 
/^ *$/{next}
start{gsub("\\\\","/");gsub("^(.?):","/cygdrive/"substr($0,1,1));printf("%s ",$0)}
/^--/{start=1}
END{print ""}
'
