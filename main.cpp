/***********************************************************************************************************************
*                                                                                                                      *
* voltage-divider-calculator v0.1                                                                                      *
*                                                                                                                      *
* Copyright (c) 2022 Andrew D. Zonenberg and contributors                                                              *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <string>

using namespace std;

void ShowHelp();

void PrintResistance(const char* name, float value);
float ParseResistance(const char* str);

int main(int argc, char* argv[])
{
	vector<float> db;
	string dbFile;

	float minSum = -1;
	float maxSum = -1;

	float minR1 = -1;
	float maxR1 = -1;

	float minR2 = -1;
	float maxR2 = -1;

	enum
	{
		GOAL_NONE,
		GOAL_DIVIDE,
		GOAL_RATIO
	} goalMode = GOAL_NONE;
	float goal = 0;

	//Parse arguments
	for(int i=1; i<argc; i++)
	{
		string s(argv[i]);

		if(s == "--database")
		{
			if(i+1 < argc)
			{
				dbFile = argv[i+1];
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--divide")
		{
			if(goalMode != GOAL_NONE)
			{
				fprintf(stderr, "Multiple goal arguments (--divide or --ratio) specified. There must be exactly one goal.\n");
				return 1;
			}
			else if(i+1 < argc)
			{
				goal = stof(argv[i+1]);
				goalMode = GOAL_DIVIDE;
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--ratio")
		{
			if(goalMode != GOAL_NONE)
			{
				fprintf(stderr, "Multiple goal arguments (--divide or --ratio) specified. There must be exactly one goal.\n");
				return 1;
			}
			else if(i+1 < argc)
			{
				goal = stof(argv[i+1]);
				goalMode = GOAL_RATIO;
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--max-sum")
		{
			if(i+1 < argc)
			{
				maxSum = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--min-sum")
		{
			if(i+1 < argc)
			{
				minSum = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--max-r1")
		{
			if(i+1 < argc)
			{
				maxR1 = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--min-r1")
		{
			if(i+1 < argc)
			{
				minR1 = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--max-r2")
		{
			if(i+1 < argc)
			{
				maxR2 = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--min-r2")
		{
			if(i+1 < argc)
			{
				minR2 = ParseResistance(argv[i+1]);
				i++;
			}
			else
			{
				ShowHelp();
				return 1;
			}
		}

		else if(s == "--help")
		{
			ShowHelp();
			return 0;
		}

		else
		{
			fprintf(stderr, "Unrecognized command-line argument \"%s\"\n", s.c_str());
			ShowHelp();
			return 1;
		}
	}

	switch(goalMode)
	{
		//Divider must be >1
		case GOAL_DIVIDE:
			if(goal < 1)
				fprintf(stderr, "Voltage dividers cannot have gain. Argument to --divide must be >= 1\n");
			break;

		//Ratio must be positive
		case GOAL_RATIO:
			if(goal <= 0)
				fprintf(stderr, "Voltage dividers cannot invert their input. Argument to --ratio must be positive");
			break;

		//Must have specified a goal
		case GOAL_NONE:
			fprintf(stderr, "No goal specified (must use one of --divide or --ratio)\n");
			return 1;

		default:
			break;
	}

	//Load database file
	if(dbFile.empty())
	{
		fprintf(stderr, "No component database file specified, use --database file.txt\n");
		return 1;
	}
	FILE* fp = fopen(dbFile.c_str(), "r");
	if(!fp)
	{
		fprintf(stderr, "Failed to open database file\n");
		return 1;
	}
	while(!feof(fp))
	{
		char line[128] = {0};
		if(nullptr == fgets(line, sizeof(line), fp))
			break;
		db.push_back(ParseResistance(line));
	}
	fclose(fp);

	//Iterate over all possible values for R1 (upper resistor)
	float closestR1 = 0;
	float closestR2 = 0;
	float closestError = FLT_MAX;
	for(auto r1 : db)
	{
		//Check constraints on R1 and skip anything not meeting them
		if( (maxR1 > 0) && ( r1 > maxR1) )
			continue;
		if( (minR1 > 0) && ( r1 < minR1) )
			continue;

		//Try all possible values for R2 (lower resistor)
		for(auto r2 : db)
		{
			//Check constraints on R2 and skip anything not meeting them
			if( (maxR2 > 0) && ( r2 > maxR2) )
				continue;
			if( (minR2 > 0) && ( r2 < minR2) )
				continue;

			//Check constraints on sum and skip anything not meeting them
			if( (maxSum > 0) && ( (r1 + r2) > maxSum) )
				continue;
			if( (minSum > 0) && ( (r1 + r2) < minSum) )
				continue;

			//Find the division ratio for the current guess
			float guess;
			if(goalMode == GOAL_DIVIDE)
				guess = (r1 + r2) / r2;
			else //if(goalMode == GOAL_RATIO)
				guess = r1 / r2;

			//See if it's better than the last try
			float error = fabs(guess - goal);

			if(error < closestError)
			{
				closestR1 = r1;
				closestR2 = r2;
				closestError = error;
			}
		}
	}

	if( (closestR1 == 0) && (closestR2 == 0) )
	{
		fprintf(stderr, "No possible solution found (empty database or unsatisfiable constraints)\n");
		return 1;
	}

	//All good, report what we found
	printf("Best solution:\n");
	PrintResistance("R1", closestR1);
	PrintResistance("R2", closestR2);
	PrintResistance("Sum", closestR1 + closestR2);
	printf("\n");

	if(goalMode == GOAL_DIVIDE)
	{
		float ratio = (closestR1 + closestR2) / closestR2;
		printf("    Goal:     divide by %7.3f\n", goal);
		printf("    Achieved: divide by %7.3f (%.1f %% error)\n", ratio, (ratio - goal) * 100 / goal);
	}

	else //if(goalMode == GOAL_RATIO)
	{
		float ratio = closestR1 / closestR2;
		printf("    Goal:     R1 / R2 = %7.3f\n", goal);
		printf("    Achieved: R1 / R2 = %7.3f (%.1f %% error)\n", ratio, (ratio - goal) * 100 / goal);
	}


	return 0;
}

/**
	@brief Converts a resistance value as a string, like "4.7k" to floating point ohms
 */
float ParseResistance(const char* str)
{
	float num;
	char suffix;
	if(2 != sscanf(str, "%f %c", &num, &suffix))
	{
		suffix = '\0';
		if(1 != sscanf(str, "%f", &num))
			return 0;
	}

	if(suffix == 'k')
		return num * 1e3;
	else if(suffix == 'M')
		return num * 1e6;
	else
		return num;
}

/**
	@brief Pretty-prints a resistance value
 */
void PrintResistance(const char* name, float value)
{
	if(value > 1e6)
		printf("    %5s = %6.2f MΩ\n", name, value * 1e-6);
	else if(value > 1e3)
		printf("    %5s = %6.2f kΩ\n", name, value * 1e-3);
	else
		printf("    %5s = %6.2f Ω\n", name, value);
}

void ShowHelp()
{
	printf("vdcalc v0.1 by Andrew D. Zonenberg\n");
	printf("\n");
	printf("Usage: vdcalc --database resistors.txt (goal) (constraints)\n");
	printf("\n");
	printf("Goal (must specify exactly one):\n");
	printf("    --divide NNN: divide by NNN\n");
	printf("    --ratio NNN:  R1 = NNN * R2\n");
	printf("\n");
	printf("Constraints (optionally, specify as many as you wish)\n");
	printf("    --max-sum NNN: sum of R1 and R2 cannot exceed NNN\n");
	printf("    --min-sum NNN: sum of R1 and R2 must exceed NNN\n");
	printf("    --max-r1 NNN:  value of R1 cannot exceed NNN\n");
	printf("    --min-r1 NNN:  value of R1 must exceed NNN\n");
	printf("    --max-r2 NNN:  value of R2 cannot exceed NNN\n");
	printf("    --min-r2 NNN:  value of R2 must exceed NNN\n");
}
