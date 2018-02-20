/*
This program will read far zone sensor data from XF for each individual of a generation and feed their theta gain values through a fitness function. The fitness scores will then be printed to handshook.csv

Hannah Hasan 11/22/2017
*/

// NOTE: there are a few commented-out cout statements in this code; this is to help trace the source of errors if any numbers seem a bit suspect.


#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


// GLOBAL VARIABLES

const int NPOP = 5;                            // individuals in each generation
const int NVALS = 6;                           // number of values given for each theta, phi coordinate in uan data file
const int THETA_INCR = 15;                     // theta step value for the simulation
const int PHI_INCR = 15;                       // phi step value for the simulation
const int HEADERLINES = 17;                    // # of header lines (not data) in the uan data file
const int NLINES = ((360/PHI_INCR)+1)*((180/THETA_INCR)+1) + HEADERLINES; // total number of lines in the file (assuming far zone sensor was for full sphere)
const int NPHIS = (360/PHI_INCR)+1;
const int THETA = 90;                          // theta coordinate we want to extract theta gain for
const double TARGETGAIN = 5.14;                // ideal gain value
const double EPSILON = 0.000001;                // if the first individual's fitness score is less than epsilon, run the program again.

// DEFINE FUNCTIONS

void Read(char* filename, ifstream& ifs, string* arr){          //reads and stores each line of data file to the array uanLines (uanLines is passed in main)

  string file = filename;
  ifs.open(file.c_str());
  if(!ifs.is_open()) cout << endl << "Error! File could not be opened" << endl << endl;
  else{
    for(int line; line<NLINES; line++){
      getline(ifs,arr[line]);
    }
    ifs.close();
    ifs.clear();
  }
}


void ExtractThetaGain(string* arr, int f, double (&thetaGain)[NPOP][NPHIS]){         // Extract theta gain values from lines beginning with THETA
  string currentLine;
  int currentTheta;
  string theta_gain;
  double gainVal;
  int thetaCounter=0;
  string first_token;
  
  for(int L=0; L<NLINES; L++){

    currentLine = arr[L];
    first_token = currentLine.substr(0, currentLine.find(' '));
    currentTheta = atoi(first_token.c_str());

    if(currentTheta == THETA){
      //cout << arr[L] << endl;

      int first_space = currentLine.find(" ");
      int second_space = currentLine.find(" ", first_space+1);
      int third_space = currentLine.find(" ", second_space+1);

      theta_gain = currentLine.substr(second_space+1, third_space - second_space);
	
      //cout << theta_gain << endl;
      gainVal = atof(theta_gain.c_str());
      thetaGain[f-1][thetaCounter] = gainVal;
      //cout << thetaGain[f-1][thetaCounter] << endl;
      thetaCounter+=1;
      //cout << thetaCounter << endl;
    }
  }
}

double Fitness(int i, double (&thetaGain)[NPOP][NPHIS]){
  // Fitness function: Sum of reciprocals of squares of differences
  /*
  double dif;
  double dif_SQ;
  double recip;
  double score = 0;
  
  for(int j=0; j<NPHIS; j++){
    dif = (TARGETGAIN - thetaGain[i][j]);
    //cout << thetaGain[i][j] << endl;
    //cout << dif << endl;
    dif_SQ = pow(dif, 2);
    recip = 1/dif_SQ;
    score += recip;
    dif = 0;
  }
  //Normalize score so that theta, phi increments don't affect score
  score = score/NPHIS;
  */

  //Fitness function: average gain value
  double score=0;
  for(int j=0; j<NPHIS; j++){
    score += thetaGain[i][j];
  }
  score=score/NPHIS;
  return score;
}


// MAIN FUNCTION

int main(int argc, char** argv){

  // Define variables here
  ifstream infile;
  string *uanLines = NULL;
  //double *thetaGain = NULL;
  //thetaGain = new double[NPOP][NPHIS];
  double thetaGain[NPOP][NPHIS];
  double *fitnessScores = NULL;


  
  if(argc != NPOP+1) cout << endl << "Error! Please specify all XF output data files. Please preserve the order of the generation's individuals." << endl << endl;
  else {

    double score;
    fitnessScores = new double[NPOP];

    while(fitnessScores[0]<EPSILON){
      
      for(int f=1; f<=NPOP; f++){

	uanLines = new string[NLINES];     // new array of strings - each index will contain a line of data file

	Read(argv[f], infile, uanLines);

	// pass uanLines for extraction of theta gain values

	ExtractThetaGain(uanLines, f, thetaGain);
      
	delete [] uanLines;
	uanLines = NULL;    
      }

      // pass theta gain values to the fitness function
      // store fitness scores in array
    
      //double score;
      //fitnessScores = new double[NPOP];                         // moved upstairs for error-checking with while loop
    
      for(int indiv=0; indiv<NPOP; indiv++){
	score = Fitness(indiv, thetaGain);
	//cout << score << endl;
	fitnessScores[indiv] = score;
      }
    }

    
    // Write handshook.csv
    ofstream handshook;
    handshook.open("handshook.csv");
    handshook << "Julie and Hannah are the coolest!" << endl;
    handshook << "Generation's fitness scores:" << endl;
    for(int i=0; i<NPOP; i++){
      handshook << fitnessScores[i] << endl;
    }
    handshook.close();
    delete [] fitnessScores;
    fitnessScores = NULL;
  }


  
  return 0;
}
