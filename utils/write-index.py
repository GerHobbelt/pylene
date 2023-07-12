#!/usr/bin/python3

import os
import re
import argparse

template_top = """
<!DOCTYPE html>
<html>

<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <meta name="Author" content="Pylene's Authors">
  <title>Pylene Documentation index by version</title>
  <style type="text/css">
    BODY {
      font-family: monospace, sans-serif;
      color: black;
    }

    P {
      font-family: monospace, sans-serif;
      color: black;
      margin: 0px;
      padding: 0px;
    }

    A:visited {
      text-decoration: none;
      margin: 0px;
      padding: 0px;
    }

    A:link {
      text-decoration: none;
      margin: 0px;
      padding: 0px;
    }

    A:hover {
      text-decoration: underline;
      background-color: yellow;
      margin: 0px;
      padding: 0px;
    }

    A:active {
      margin: 0px;
      padding: 0px;
    }

    .VERSION {
      font-size: small;
      font-family: arial, sans-serif;
    }

    .NORM {
      color: black;
    }

    .FIFO {
      color: purple;
    }

    .CHAR {
      color: yellow;
    }

    .DIR {
      color: blue;
    }

    .BLOCK {
      color: yellow;
    }

    .LINK {
      color: aqua;
    }

    .SOCK {
      color: fuchsia;
    }

    .EXEC {
      color: green;
    }
  </style>
<body>
"""

template_bottom = """
</body>

</html>
"""

replace_dict = {
  re.compile(r"master", re.IGNORECASE): "stable",
  re.compile(r"next", re.IGNORECASE): "unstable",
  re.compile(r"v?-?([0-9]+(?:\.[0-9]+)*(?:-?[a-zA-Z0-9_-]+)?)", re.IGNORECASE): "version-\\1",
}


def apply_replacements(s, replacements):
  for pattern, replacement in replacements.items():
    s = pattern.sub(replacement, s)
  return s


def main(public_dir, index_file, needle_file):

  # Directory to start searching from
  start_dir = 'public/'

  # The file to write the results to
  output_file_path = os.path.join(public_dir, index_file)

  # Open the output file
  with open(output_file_path, 'w') as outfile:
    outfile.write(template_top)
    outfile.write("<h1>Pylene Documentation index by version</h1>\n<hr>")
    outfile.write("<div>\n<ul>\n")

    # Iterate over all direct subdirectories in start_dir
    for item in os.listdir(start_dir):
      item_path = os.path.join(start_dir, item)

      # If the item is a directory
      if os.path.isdir(item_path):
        index_file_path = os.path.join(item_path, needle_file)

        # If there is an index.html file in the directory
        if os.path.isfile(index_file_path):
          # Write a hyperlink to the index file in the output file
          relative_link = os.path.join(item, needle_file)
          displayed = apply_replacements(item, replace_dict)
          outfile.write(f'<li><a href="{relative_link}">{displayed}</a></li>\n')
    outfile.write("</ul>\n</div>\n<hr>")
    outfile.write(template_bottom)


if __name__ == "__main__":
  parser = argparse.ArgumentParser(
    description="Look into public-dir for nested directories containing needle-file,"
                " then output an index-file listing and linking to those nested needle-files.")

  parser.add_argument("--public-dir",
                      help="The public working directory",
                      default="public/")
  parser.add_argument("--index-file",
                      help="The output index file",
                      default="index.html")
  parser.add_argument("--needle-file",
                      help="The needle file",
                      default="index.html")
  args = parser.parse_args()

  main(public_dir=args.public_dir, index_file=args.index_file, needle_file=args.needle_file)
