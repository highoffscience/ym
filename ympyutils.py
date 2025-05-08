##
# @file    ympyutils.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import subprocess as sp

def runCmd(
   cmd: str,
   cwd: str             = ".",
   per_line_action_func = None,
   quiet                = False):
   """
   @brief Runs command and optionally runs an action function on every line of the output.

   @param cmd                  -- Command to run.
   @param cwd                  -- Directory to run command in.
   @param per_line_action_func -- Action function to run on every line of output.
   @param quiet                -- Whether or not to suppress the return code.

   @returns str -- Output of command (if no action function defined).
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
   **kwargs) :
   """
   TODO
   """

   dirname = os.path.dirname(fullfilename)
   if dirname:
      os.makedirs(dirname, exist_ok=True)
   return open(fullfilename, *args, **kwargs)
