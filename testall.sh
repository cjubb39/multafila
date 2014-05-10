# bash
obj="./mulf -t"
log="test-log.log"
error=0

rm -f $log

Usage(){
    echo "Usage: testall.sh [options] [.mulf files]"      
        echo "-h    Print this help"
        exit 1
}

Compare() {
    
    diff -b $1 $2  1>&2
    diff -b "$1" "$2" > "$3" 2>&1 || {  
    error=1
    echo "FAILED $1 differs from $2" 1>&2
    }
}

Run(){
    echo $* 1>&2
        eval $* || {
        echo "Test case failed on $*" 1>&2
        
        if [ $error -eq 0 ]  
            then error=1
        fi
        return 1
        }
}

Check(){
    casename=`echo $1 | sed 's/.*\///
                             s/.mulf//'`
     reffile=`echo $1 | sed 's/.mulf$/.out/'`
    


    echo 1>&2   
    echo "========= Test Case $casename===========================================" 1>&2


    outfile=$casename.out
    
    Run $obj $1 1>&2

    if [ $error -eq 0 ]
    then
        echo "PASSED!" 1>&2
        echo "$casename PASSED =============>Expected to Pass"
        else
        echo "FAILED!" 1>&2
        echo "$casename FAILED =============>Expected to Pass"
    
        fi
    rm -f $outfile $casename.out.diff
}

while getopts h opt
do
    case $opt in
     h) # print usage
        Usage;;
    esac
done

# digest options
shift `expr $OPTIND - 1`


if [ $# -ge 1 ]
then    files=$@
else 
    files="tests/*.mulf"
fi

date>>$log
echo >>$log
    
for file in $files
do
     case $file in
    *test-*)
        Check $file 2>>$log;;
    *)
        echo "$file =============>NOT CHECKED";;
     esac
    error=0;        
done

exit 0 