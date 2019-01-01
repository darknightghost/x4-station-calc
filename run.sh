#! /bin/bash

MAIN_DIR=`realpath \`dirname $0\``
${MAIN_DIR}/test_qt.py

if [[ $? -ne 0 ]]; then
	pip3 install pyqt5
	${MAIN_DIR}/test_qt.py
	if [[ $? -ne 0 ]]; then
		echo "Failed io install pyqt5."
		exit -1

	fi
fi

${MAIN_DIR}/main.py $0
