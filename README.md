ym is meant to be a common repository and way to structure c++ projects.
Framework is still in progress. ym is meant to be added as a subtree,
since modification/addition to specific project needs is expectd.

Install instructions

To update submodules (cppyy and fmt) run
git submodule update --init --recursive

To update subtree (ym) run
git subtree pull --prefix ym <git-url> <branch-name> --squash

url-ym = "https://github.com/highoffscience/ym.git" on branch "main"

Use strlit over str to indicate the pointer can be stored. strlit has program-long lifetime,
   so it is safe to store a reference.
