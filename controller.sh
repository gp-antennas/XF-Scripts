#!/bin/bash

module load xfdtd #this loads the XF module when running Jordan's code through the Ohio supercomputers. If you are not running that way, you need to change this to load XFdtd locally. Repace this with your local call XF command. 

cd Evolved_Dipole #move to Evolved_Dipole Directory

./Evolved_Dipole --start #Create First Gen

while inotifywait ~/Evolved_Dipole/watches -r -e modify -m; do #wait until watches directory is modified
    if tail -n1 ~/Evolved_Dipole/watches/watch.txt | grep 0; then #if a 0 is written to watch.txt then run xf macro
	xfui --execute-macro-script=/users/PAS0654/osu8742/XFScripts/dipole_PEC.xmacro #this location is the location for the supercomputers. This also needs to be changed to the local directory with this macro.
    elif tail -n1 ~/Evolved_Dipole/watches/watch.txt | grep 1; then #if a 1 has been written to watch.txt then make Gen 2 or higher
	./Evolved_Dipole --cont
   # else #if anything else is written then end
    fi
done
