#!/bin/bash
#Loop through several generations of antennas using Evolved_Dipole and XF
#Created on 12/06/2017
#Hannah Hasan


####### LINES TO CHECK OVER WHEN STARTING A NEW RUN #######

RunName='EvDip_2'                ## Replace when needed
TotalGens=10   ## number of generations (after initial) to run through
NPOP=5       ## number of individuals per generation; please keep this value below 99

###########################################################


WorkingDir=`pwd`
XmacrosDir=$WorkingDir/../Xmacros
XFexec='/mnt/c/Users/Hannah/Documents/Research/XF/bin/Win64.NET2010'  ##Location of XFUI executable
#XFProj='/mnt/c/Users/Hannah/Documents/Research/XF/ARA_XFProjects_misc/Projects/'$RunName'.xf/'    ##give path to the directory you will save the XF project in. Additionally, please save the XF project with the same name as RunName
XFProj=$WorkingDir/../Projects/$RunName.xf  ## Provide path to the project directory in the 'single quotes'


## Lines for output.xmacro files ##
line1='var query = new ResultQuery();'
line2='///////////////////////Get Theta and Phi Gain///////////////'
line3='query.projectId = App.getActiveProject().getProjectDirectory();'
line4='query.simulationId = '  ## append 6-digit simulation ID number; for example "000001";
## cat skeleton here
lastline='FarZoneUtils.exportToUANFile(thdata,thphase,phdata,phphase,inputpower,"C:/Users/Hannah/Documents/Research/XF/XF_Scripts/Evolved_Dipole/data/' ##append simulation ID number followed by .uan");


cd hands
mkdir $RunName
cd $RunName
mkdir plots

cd "$WorkingDir"
rm highfive.txt
echo 0 >> highfive.txt


##### Select ONE evolution program #####

./Evolved_Dipole.exe --start
#./handflail.exe --start
#./Roulette_Select.exe --start

########################################


cp handshake.csv hands/$RunName/0_handshake.csv

cd $XmacrosDir
rm output.xmacro
rm dipole_PEC.xmacro

cd "$WorkingDir"
cd data
for ((indiv=1; indiv<=$NPOP; indiv++))
do
    echo "$line1" >> $XmacrosDir/output.xmacro
    echo "$line2" >> $XmacrosDir/output.xmacro
    echo "$line3" >> $XmacrosDir/output.xmacro
    if [ "$indiv" -lt 10 ]
    then
	echo "$line4"'"00000'"$indiv"'";'>> $XmacrosDir/output.xmacro
    elif [ "$indiv" -ge 10 ]
    then
	echo "$line4"'"0000'"$indiv"'";'>> $XmacrosDir/output.xmacro
    fi
    cat outputmacroskeleton.txt >> $XmacrosDir/output.xmacro
    echo "${lastline}${indiv}"'.uan");' >> $XmacrosDir/output.xmacro
done

cat dipolePECmacroskeleton.txt >> $XmacrosDir/dipole_PEC.xmacro
echo 'for(var i = 0;i < '"$NPOP"';i++){' >> $XmacrosDir/dipole_PEC.xmacro
cat dipolePECmacroskeleton2.txt >> $XmacrosDir/dipole_PEC.xmacro

echo
echo
echo 'Opening XF user interface...'
echo '*** Please remember to save the project with the same name as RunName! ***'
echo
echo '1. Import and run dipole_PEC.xmacro'
echo '2. Import and run output.xmacro'
echo '3. Close XF'
read -p "Press any key to continue... " -n1 -s

## Run XF -- opens graphical interface. From there, dipole_PEC.xmacro needs to be run, then all output(...).xmacro scripts (1-5) shsould be run.
cd $XFexec
./xfui.exe
echo
read -p "Press any key to continue... " -n1 -s
echo
echo "Resuming..."
echo

cd "$WorkingDir"
cd data
InputFiles=
for ((i=1; i<=NPOP; i++))
do
    InputFiles="${InputFiles}${i}.uan "
done
./makehandshook.exe $InputFiles
mv handshook.csv "$WorkingDir"
cd "$WorkingDir"

## make plots
cd "$WorkingDir"
rm gensData.csv
./gensData.exe
for ((i=1; i<=NPOP; i++))
do
    cp data/$i.uan ${i}uan.csv
    cp data/$i.uan "$WorkingDir"/hands/$RunName/gen0_${i}.uan
done

./uanCleaner.exe

python plotLR.py
#python PlotGainPat.py

mv Length.png "$WorkingDir"/hands/$RunName/plots
mv Radius.png "$WorkingDir"/hands/$RunName/plots

for ((i=1; i<=NPOP; i++))
do
    mv ${i}uan.csv "$WorkingDir"/hands/$RunName/plots/0_${i}uan.csv
#    mv ${i}uan.png "$WorkingDir"/hands/$RunName/plots/0_${i}uan.png
done

cp handshook.csv hands/$RunName/0_handshook.csv

for ((gen=1; gen<=$TotalGens; gen++)); do                                 # use for fixed number of generations
#gen=0; while [ `cat highfive.txt` -eq 0 ]; do (( gen++ ))                 # use for runs until convergence

    cd "$WorkingDir"
    ./Evolved_Dipole.exe --cont
    #./handflail.exe --cont
    #./Roulette_Select.exe --cont

    cp handshake.csv hands/$RunName/${gen}_handshake.csv
    

#    let simID=$(( $gen*5+1 ))
#    #### FIX NUMBERS SO THEY MATCH SIMULATION ID ####
#    for((ID=$simID; ID<simID+5; ID++))
#    do
#	echo $line1 >> $XmacrosDir/output_${gen}.xmacro
#	echo $line2 >> $XmacrosDir/output_${gen}.xmacro
#	echo $line3 >> $XmacrosDir/output_${gen}.xmacro
#    if(("$ID" < 10))
#	then
#	    echo $line4'"00000'${ID}'";' >> $XmacrosDir/output_${gen}.xmacro
#	elif(("$ID" >= 10 && "$ID" < 100 ))
#	then
#	    echo $line4'"0000'${ID}'";' >> $XmacrosDir/output_${gen}.xmacro
#	else
#	    echo $line4'"000'${ID}'";' >> $XmacrosDir/output_${gen}.xmacro
#	fi
#	cat outputmacroskeleton.txt >> $XmacrosDir/output_${gen}.xmacro
#	echo ${lastline}${i}'.uan");' >> $XmacrosDir/output_${gen}.xmacro
#    done

    cd $XFexec
    cd $XFProj
    rm -rf Simulations
    echo
    echo
    echo 'Opening XF user interface...'
    echo 
    echo '1. Run dipole_PEC.xmacro (no new modifications needed)'
    echo "2. Run output.xmacro (no new modifications needed)"
    echo '3. Close XF'
    read -p "Press any key to continue... " -n1 -s
    echo
    ## Run XF -- opens graphical interface. From there, dipole_PEC.xmacro needs to be run, then all output(...).xmacro scripts (1-5) shsould be run.
    cd $XFexec
    ./xfui.exe $XFProj
    
    read -p "Press any key to continue... " -n1 -s
    echo "Resuming..."
    echo


    cd "$WorkingDir"
    cd data
    InputFiles=
    for ((i=1; i<=NPOP; i++))
    do
	InputFiles="${InputFiles}${i}.uan "
    done
    ./makehandshook.exe $InputFiles
    mv handshook.csv "$WorkingDir"
    cd "$WorkingDir"


    ## make plots
    cd "$WorkingDir"
    ./gensData.exe
    for ((i=1; i<=NPOP; i++))
    do
	cp data/$i.uan ${i}uan.csv
	cp data/$i.uan "$WorkingDir"/hands/$RunName/gen${gen}_${i}.uan
    done

    ./uanCleaner.exe
    
    python plotLR.py
    #python PlotGainPat.py

    mv Length.png "$WorkingDir"/hands/$RunName/plots
    mv Radius.png "$WorkingDir"/hands/$RunName/plots

    for ((i=1; i<=NPOP; i++))
    do
	mv ${i}uan.csv "$WorkingDir"/hands/$RunName/plots/${gen}_${i}uan.csv
#	mv ${i}uan.png "$WorkingDir"/hands/$RunName/plots/${gen}_${i}uan.png
    done

    
#    cd "$WorkingDir"
#    cd data
#    InputFiles=
#    for((ID=1; ID<=5; ID++))
#    do
#	InputFiles=${InputFiles}${ID}'.uan '
#    done
#    ./makehandshook.exe $InputFiles
#    mv handshook.csv "$WorkingDir"
#    cd "$WorkingDir"
    
    cp handshook.csv hands/$RunName/${gen}_handshook.csv
    
done


echo
echo 'Done!'
