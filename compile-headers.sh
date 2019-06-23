#!/bin/bash

header_name="./containers.h"
IFS=''
first="1"

find ./src -type f -name "*.h" | while read header;
do
	ignore="1"
	if [[ $first == "1" ]];
	then
		first="0"
		cp "./src/include/VERSION" "$header_name"
	fi
	while read line;
	do
		if [[ $ignore == "0" ]];
		then
			echo "$line" >> "$header_name"
		elif [[ $line == *"*/"* ]];
		then
			ignore="0"
			echo "" >> "$header_name"
		fi
	done < "$header"
done
