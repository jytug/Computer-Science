#!/bin/bash

if [ $# == 2 ] && [ -e $1 ] && [ -d $2 ]; then
	for f in $2/*.in; do
		if [ -f ${f%.in}.out ]; then
			./$1 <$f >${f%.in}.res
			if diff ${f%.in}.res ${f%.in}.out >/dev/null ; then
				echo "$(tput setaf 2)Test $f passed$(tput sgr0)"
			else
				echo "$(tput setaf 1)Test $f failed$(tput sgr0)"
			fi
			rm ${f%.in}.res
		else
			echo "The \"$2\" directory doesn't hold the desired structure"
		fi
	done
else
	echo "Wrong execution parameters"
fi
