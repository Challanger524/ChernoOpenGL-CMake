# Note: Some commands are Linux only. If you are a Windows user - `WSL` is the best pick to obtain Linux command line and the all power it unleshes:)
# Note: MS Windows powershell/cmd capabilities are thousand times inferrior to GNU Linux command line and it's tools, just bitter reality

# General commands
git rev-list --count HEAD # count commits (check that no commits were accidentally lost/squashed)
git ls-files -- . ':!:Dependencies/' ':!:Documents/'
git diff --name-status main | column -t --table-columns='Operation,Old path,New path'

# History rewriting (manual):
git rebase --interactive <commit>~ # start rebasing from <commit>
git rebase --interactive --root    # allows to pick first commit (alike previous command)
git add --update             # update already tracked files
git commit --amend --no-edit #! do not commit on merge conflicts! u need just to `add` fixes and `rebase --continue`
git rebase --continue        # go to next commit marked for editing or merge-conflict to resolve

# Submodules moving/renaming
git mv <submodule_path_old>[/] <submodule_path_new>[/] # on moving one dir lower with same name - additional temp move must be performed
git rm <submodule_path> # the only way to rename (in .gitmodules) - is to **remove** and to **add** againg
git submodule add --depth 1 --name <name> [-b <branch>] -- <url> [<path>] # important to specify short (path-less) name # do not use `--depth` with `-b`

# If commit accidentally squashed while fixing merge-conflict and u don't want to start from last _checkpoint_:
git format-patch -1 <hash> # patch can be produced from saved branch
git apply <*.patch>        # and from previous (infront of squashed) commit can be re-created, then restore description of next commit

git reset --hard <dev_branch> # replace main_branch with dev_branch, if contains (valid!) updated history

# External **git-filter-repo** (python3) tool must be installed: https://github.com/newren/git-filter-repo
git filter-repo --dry-run --refs <branch> --path-rename Dependencies/**:deps/**                # rename/move dirictories
git filter-repo --dry-run --refs <branch> --replace-text <(echo 'regex:Dependencies/==>deps/') # simple find&replace
git filter-repo --dry-run --refs <branch> --replace-text <(echo 'regex:^\xEF\xBB\xBF==>')      # strip UTF-8 BOM (freaking side effect of using Visual Studio)
git filter-repo --dry-run --refs <branch> --replace-text <(echo 'regex:\n\n==>\n') # same as below but applies on the all (damn) tracked files (file/path specify not supported)

# replace 2+ blank lines with only 1, while targeting specified `files`/`extensions` in specified `paths`. **perl** has the most sane replacing syntax (alike: sed, awk...)
git filter-branch --tree-filter "find . -path './src/*' -regextype egrep -regex '.*\.(hpp|cpp)' -exec perl -0777 -pe 's{\n\n\n+}{\n\n}g' -i {} \;" <test_branch/HEAD/HEAD~..HEAD>
