#!/bin/bash

set -x

if [ -z "${DOC_PERSIST_BRANCHES}" ]; then
  echo "PERSIST_BRANCHES is not set. Please set this variable and try again."
  exit 1
fi

if [ -z "${CURRENT_BRANCH}" ]; then
  echo "CURRENT_BRANCH is not set. Please set this variable and try again."
  exit 1
fi

if [ -z "${CI_PROJECT_ID}" ]; then
  echo "CI_PROJECT_ID is not set. Please set this variable and try again."
  exit 1
fi

if [ -z "${ARTIFACT_RETRIEVER}" ]; then
  echo "ARTIFACT_RETRIEVER is not set. Please set this variable and try again."
  exit 1
fi

IFS=':' read -ra BRANCHES <<<"$DOC_PERSIST_BRANCHES"
# For every persistant branches except the current one
for i in "${BRANCHES[@]}"; do
  if [ "$i" != "$CURRENT_BRANCH" ]; then
    echo "Processing branch: $i"
    # Downloads artifacts
    curl --location --output ./artifacts_$i.zip --header "PRIVATE-TOKEN: $ARTIFACT_RETRIEVER" "https://gitlab.lre.epita.fr/api/v4/projects/$CI_PROJECT_ID/jobs/artifacts/$i/download?job=pages"
    # Test artifacts
    if ! unzip -tq ./artifacts_$i.zip >/dev/null 2>&1; then
      echo "Invalid archive, ignoring for branch <$i>..."
    else
      # Extract into /public/branch_name
      echo "Found valid archive for branch <$i>, restoring..."
      unzip -o ./artifacts_$i -d .
      echo "Restored documentation for <$i>"
      rm -rf *.json artifacts_$i artifacts_$i.zip
    fi
  fi
done
