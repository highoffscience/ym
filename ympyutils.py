##
# @file    ympyutils.py
# @version 1.0.0
# @author  Forrest Jablonski
#

"""
   Utility functions.
"""

import os
import subprocess as sp

def runCmd(
   cmd: str,
   cwd: str             = ".",
   per_line_action_func = None,
   quiet: bool          = False) -> str:
   """
   Runs command and optionally runs an action function on every line of the output.

   Args:
      cmd:                  Command to run.
      cwd:                  Directory to run command in.
      per_line_action_func: Action function to run on every line of output.
      quiet:                Whether or not to suppress the return code.

   Returns:
      Output of command (if no action function defined).
   """
   
   output = None
   with sp.Popen(cmd.split(), stdout=sp.PIPE, stderr=sp.STDOUT, text=True, cwd=cwd) as p:
      if per_line_action_func:
         for line in iter(p.stdout.readline, ""):
            per_line_action_func(line.strip())
      else:
         output, _ = p.communicate()
      if not quiet:
         print(f"Command '{cmd}' exited with code {p.poll()}")
   return output

def open_into_dir(
   fullfilename: str,
   *args,
   **kwargs):
   """
   Opens a file and creates all directories within the qualified path.

   Args:
      fullfilename: Fully qualified file name.
      *args:        Self explanatory.
      **kwargs:     Self explanatory.

   Returns:
      io: The opened file.
   """

   dirname = os.path.dirname(fullfilename)
   if dirname:
      os.makedirs(dirname, exist_ok=True)
   return open(fullfilename, *args, **kwargs)
