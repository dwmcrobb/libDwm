#!/bin/sh

TAGNAME=$1

echo "${TAGNAME}" > latest_release
git add latest_release
git commit -m "Tagging as ${TAGNAME}"
git tag -a "${TAGNAME}" -m "Tagged as ${TAGNAME}"
git push --tags
