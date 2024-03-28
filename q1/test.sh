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

specialChars="!@#$%^&*()_+{}|:<>?[]\;',./"

## read the file word by word
text="This is a sample text to iterate word by word"

# Use a while loop with read command to iterate over each word

while read -r word; do
    echo "Word: $word"
done






