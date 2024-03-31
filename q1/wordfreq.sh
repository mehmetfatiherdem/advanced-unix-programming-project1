#!/bin/bash

## check if the user specified a file
## if not, read from standard input

if [[ $# -eq 0 ]]
then
    echo "Enter the text, Ctrl-D to end: "
    cat > stdinput
    set stdinput
fi

echo -n "Enter the minimum occurrence threshold:"
read th
echo -n "Enter the number of top words to display:"
read top

echo
echo >> output.txt

wordsArr=()
freqArr=()

total=0
numOfWords=0

# exclude the numbers, articles,prepositions, and conjunctions (e.g., "the", "and", "of").
exclude="the and of a an in on to for with by as at from into through over under above below between among around behind beside down during except inside near off out outside since until upon within without"

## read the file word by word
for word in `cat $1 | grep -o -E '\w+'`
do

    ## exclude the words in the exclude list
    if [[ $(echo $exclude | grep -i $word) ]]
    then
        continue
    fi

    found=0

    ## check if the word is already in the array
    ## if so, increment its frequency
    for (( i=0; i<${#wordsArr[@]}; i++ ))
    do
        if [[ $(echo ${wordsArr[$i]} | grep -i $word) ]]
        then
            freqArr[$i]=$((${freqArr[$i]}+1))
            found=1
            break
        fi
    done

    ## if the word is not in the array add it
    if [[ $found -eq 0 ]]
    then
        wordsArr+=($word)
        freqArr+=("1")
    fi
done



## sort the array by frequency
for (( i=0; i<${#freqArr[@]}; i++ ))
do
    for (( j=0; j<${#freqArr[@]}; j++ ))
    do
        if [[ ${freqArr[$i]} -gt ${freqArr[$j]} ]]
        then
            temp=${freqArr[$i]}
            freqArr[$i]=${freqArr[$j]}
            freqArr[$j]=$temp

            temp=${wordsArr[$i]}
            wordsArr[$i]=${wordsArr[$j]}
            wordsArr[$j]=$temp
        fi
    done
done

if [[ -f output.txt ]]
then
    rm output.txt
fi

## print the top words
echo "Word Frequency Report:"
echo "Word Frequency Report:" >> output.txt
for (( i=0; i<$top; i++ ))
do
    if [[ ${freqArr[$i]} -ge $th ]]
    then
        echo "${wordsArr[$i]} ${freqArr[$i]}"
        echo "${wordsArr[$i]} ${freqArr[$i]}" >> output.txt
        numOfWords=$[$numOfWords+${freqArr[$i]}]
        total=$[$total+${#wordsArr[$i]}*${freqArr[$i]}]
    fi
done


echo
echo >> output.txt

## print average word length
average=$(echo "scale=2; $total / $numOfWords" | bc)

echo "Average Word Length: $average"
echo "Average Word Length: $average" >> output.txt

echo
echo >> output.txt

echo "Histogram of Word Frequencies:"
echo "Histogram of Word Frequencies:" >> output.txt

## print the histogram
for (( i=0; i<$top; i++ ))
do  
    if [[ ${freqArr[$i]} -lt $th ]]
    then
        continue
    fi
    echo -n -e "${wordsArr[$i]}\t"
    echo -n -e "${wordsArr[$i]}\t" >> output.txt

    for (( j=0; j<${freqArr[$i]}; j++ ))
    do
        echo -n "*"
        echo -n "*" >> output.txt
    done

    echo
    echo >> output.txt

done


