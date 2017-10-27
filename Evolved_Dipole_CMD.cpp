/* 
 * File:   Evolved_Dipole.cpp
 * Author: Jordan Potter
 *
 * Created on July 14, 2017, 11:07 AM
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int NVAR=2; //number of variables (r and l)
const int NPOP=5; //size of population


double rand_normal(double mean, double stddev)
{//Box muller method to generate normal numbers
    static double n2 = 0.0;
    static int n2_cached = 0;
    if (!n2_cached)
    {
        double x, y, r;
        do
        {
            x = 2.0*rand()/RAND_MAX - 1;
            y = 2.0*rand()/RAND_MAX - 1;

            r = x*x + y*y;
        }
        while (r == 0.0 || r > 1.0);
        {
            double d = sqrt(-2.0*log(r)/r);
            double n1 = x*d;
            n2 = y*d;
            double result = n1*stddev + mean;
            n2_cached = 1;
            return result;
        }
    }
    else
    {
        n2_cached = 0;
        return n2*stddev + mean;
    }
}



void GenerateSample(double new_val[][NVAR], double mean_val[], double std_dev[],int n){
    double u;
    for(int i=0;i<NVAR;i++)
    {
        //Create a sample of mean + stdev*(gaussian sample) to use
        u=rand_normal(0,.5);
        new_val[n][i]=mean_val[i]+std_dev[i]*u;
        
        //Include conditionals for bounds and constraints here
        int errors=0;
        if(new_val[n][i] <= 0)
        {
            i--; //If a parameter is less than 0, decrement so this value is created again
            errors++;
        }
        
        if(errors >= 50) //If there are 50 errors something is going wrong, so stop
        {
            cout << "Error: Sample Generation Failed" << endl;
            return;
        }
    }
}
void Simulation(double x[][NVAR], double y[NVAR], double mean[], double deviation[], double best[]){
    //Write to CSV file. Might need to slightly adjust for generations after #1...
    //For now, this will only write the last generation to file
    ofstream handshake;
    handshake.open("handshake.csv");
    handshake << "C++ ESTRA -- Jordan Potter" << "\n";
    handshake << "Mean Vector:" << "\n"; //Write the Current Mean Values to handshake.csv
    for(int i=0;i<NVAR;i++)
      {
	if(i==(NVAR-1))
	  {
	    handshake << mean[i] << "\n";
	  }
	else
	  {
	    handshake << mean[i] << ",";
	  }
      }
    handshake << "Deviation Vector:" << "\n"; //Write the Current Deviation Values to handshake.csv
    for(int i=0;i<NVAR;i++)
      {
	if(i==(NVAR-1))
	  {
	    handshake << deviation[i] << "\n";
	  }
	else
	  {
	    handshake << deviation[i] << ",";
	  }
      }
    handshake << "Best Antenna Vector:" << "\n"; //Write the Current Deviation Values to handshake.csv
    for(int i=0;i<(NVAR+1);i++)
      {
	if(i==(NVAR))
	  {
	    handshake << best[i] << "\n";
	  }
	else
	  {
	    handshake << best[i] << ",";
	  }
      }
    handshake << "Generation:" << "\n"; //Write the Generation Values to handshake.csv
    for(int i=0;i<NPOP;i++)
    {
        for(int j=0;j<NVAR;j++)
        {
            if(j==(NVAR-1))
            {
                handshake << x[i][j] << "\n";
            }
            else
            {
                handshake << x[i][j] << ",";
            }
        }
    }
    handshake.close();

    //Set Value in another file, so that controller.sh knows to start XF
}

void Read(double x[][NVAR], double y[NVAR], double mean[], double deviations[], double best[]){    
    //Import values from Handshake for mean/x population/current deviations
  ifstream handshake;
  handshake.open("handshake.csv");
  string csvContent[NPOP+8]; //contain each line of csv
  string strToDbl; //gets overwritten to contain each cell of a line. Then transferred to x,y,mean,deviations,best
  for(int i=0;i<(NPOP+8);i++)
    {
      getline(handshake,csvContent[i]); //read in mean
      if(i==2)
	{
	  istringstream stream(csvContent[i]);
	  for(int j=0;j<NVAR;j++)
	    {
	      getline(stream,strToDbl,',');
	      mean[j] = atof(strToDbl.c_str());
	    }
	}
      else if(i==4) //read in deviations
	{
	  istringstream stream(csvContent[i]);
	  for(int j=0;j<NVAR;j++)
	    {
	      getline(stream,strToDbl,',');
	      deviations[j] = atof(strToDbl.c_str());
	    }
	}
      else if(i==6) //read in the current best values
	{
	  istringstream stream(csvContent[i]);
	  for(int j=0;j<NVAR+1;j++)
	    {
	      getline(stream,strToDbl,',');
	      best[j] = atof(strToDbl.c_str());
	    }
	}
      else if(i>=8) //read in the generation
	{
	  istringstream stream(csvContent[i]);
	  for(int j=0;j<NVAR;j++)
	    {
	      getline(stream,strToDbl,',');
	      x[i-8][j] = atof(strToDbl.c_str());
	    }
	}
    }


    //Import Values from Simulation from Handshook that XF (or eventually ARASim) will write
    //May need to adjust if format of handshook changes
  ifstream handshook;
  handshook.open("handshook.csv");
  string strToDbl2[NPOP+2];
  for(int i=0;i<(NPOP+2);i++)
    {
      getline(handshook,strToDbl2[i]);
      if(i>=2)
        {
	  y[i-2] = atof(strToDbl2[i].c_str());
        }
    }
  
  handshook.close();
}

int Mutate(double x[][NVAR],double out[], double best[], double mean[]){
    //check to see if the output from x[i][] is larger than the output from x[m].
    //i.e. output[i]>mean_output
    int count=0;
    for(int i=0;i<NPOP;i++)
    {
        if(out[i]>best[0])
        {
            best[0]=out[i];
            for(int j=0;j<NVAR;j++)
            {
                best[j+1]=x[i][j];
                mean[j]=x[i][j];
            }
            count++;
        }
    }
    return count;
}
void CheckConvergence(int numSuccess, double p, double q, double d[]){
    //If things aren't getting better than expand search radius. 
    //However, if things are getting better tighten search radius.
    //Check this over k? values of x.
    //P(output)>p then d=d/q
    //if not then d=d*q
    double P = (double) numSuccess/NPOP;
    if(P >= p)
    {
        for(int i=0;i<NVAR;i++)
        {
            d[i]=d[i]/q;
        }
    }
    else
    {
        for(int i=0;i<NVAR;i++)
        {
            d[i]=d[i]*q;
        }
    }
}


int main(int argc, char** argv) {
    double m[NVAR]={.5,.5}; //mean values of r and l
    double x[NPOP][NVAR]; //produced value of r and l
    double d[NVAR] = {.25,.25}; //standard deviation vector
    double best[NVAR+1]={-40,0,0}; //array to store param values and output score of top antenna {fit_score,r,l}
    double output[NPOP]; //array to store scores of each generations
    double q = .9; //factor to adjust search radius by
    double p = .2; //ratio of samples that must be correct to adjust search radius
    int numSuccess; //number of samples that improve on the best value
    srand(time(0));
    
    if(argc != 2)
        cout << "Error: Usage. Specify --start or --cont" << endl;
    else
    {
      if(string(argv[1]) == "--start")
      {

	for(int i=0;i<NPOP;i++)
	  {
	    GenerateSample(x,m,d,i); //Generate NPOP samples
	  }
	Simulation(x, output, m, d, best); // Write current population to disk as well as mean,deviations and best population
      }
      else if(string(argv[1]) == "--cont")
        {
	  //Read Function (read in population to x[],mean to m[],deviations to d[], best to best[] and scores to output[]. 
	  Read(x,output,m,d,best);
	  numSuccess = Mutate(x, output, best, m);
	  CheckConvergence(numSuccess, p, q, d);
	  

	  //Check if d/d_o<lambda has been achieved. Consider adding this to check convergence function. 
	  
	  cout << "The best parameter values are:" << endl;
	  for(int i =0; i <NVAR; i++)
	    {
	      cout << m[i] << endl; //Consider writing to a log file
            }

	  for(int i=0;i<NPOP;i++)
	    {
	      GenerateSample(x,m,d,i);
	    }
	  Simulation(x, output, m, d, best);
        }
      else
        {
	  cout << "Error: Specify --start or --cont" << endl;
        }
    }
    return 0;
}
