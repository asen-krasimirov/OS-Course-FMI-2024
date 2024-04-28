#!/bin/bash

while read file; do
	newName="$(echo ${file} | sed -E s/txt/sh/)"
	echo "New name: ${newName}"
	mv ${file} ${newName}
done < <(find . -regex '.*\.txt')
