#/bin/sh

PATH=/opt/git:$PATH

type git

if [ -z "$1" ];
then
	cd /opt/git
	make && cd t && sh ./t7503-pre-commit-hook.sh -v -d
else
	cd /opt/git
	make && cd /opt/project && git commit --amend --no-edit
	echo "$?"
fi
