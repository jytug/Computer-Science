#!/bin/bash

PARAM="dummy"
if [ $# == 2 ] || [ $# == 3 ]; then
	if [ $# == 3 ]; then
		PARAM=$1
		PROG=$2
		DIR=$3
	elif [ $# == 2 ]; then
		PARAM="dummy"
		PROG=$1
		DIR=$2
	fi
	
	if [ -e $PROG ] && [ -d $DIR ]; then
		for f in $DIR/*.in; do
			if [ -f ${f%.in}.out ]; then
				if [ $PARAM == "-v" ]; then
					./$PROG $PARAM <$f 1>dummy.out 2>dummy.err
				else
					./$PROG <$f >dummy.out
				fi
				if diff dummy.out ${f%.in}.out >/dev/null ; then
					echo "$(tput setaf 2)Test $f passed (stdout)$(tput sgr0)"
				else
					echo "$(tput setaf 1)Test $f failed (stdout)$(tput sgr0)"
				fi
				if [ $PARAM == "-v" ]; then
					if diff dummy.err ${f%.in}.err >/dev/null ; then
						echo "$(tput setaf 2)Test $f passed (stderr)$(tput sgr0)"
					else
						echo "$(tput setaf 1)Test $f failed (stderr)$(tput sgr0)"
					fi
				fi
				rm dummy.out 
				if [ -e dummy.err ]; then
					rm dummy.err
				fi
			else
				echo "The \"$2\" directory doesn't hold the desired structure"
			fi
		done
	else
		echo "Wrong execution parameters"
	fi
else
	echo "Wrong number of parameters"
fi
