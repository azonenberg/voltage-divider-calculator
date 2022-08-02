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
#include <set>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <string>

using namespace std;

void ShowHelp();

void PrintResistance(const char* name, float value);
float ParseResistance(const char* str);
void AddStandardValues(set<float> *db, const float *values, int n);

// Standard E* values.
const float e6[] =
{
	1.0, 1.5, 2.2, 3.3, 4.7, 6.8
};
const float e12[] =
{
	1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2
};
const float e24[] =
{
	1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1
};
const float e48[] =
{
	1.00, 1.05, 1.10, 1.15, 1.21, 1.27, 1.33, 1.40, 1.47, 1.54, 1.62, 1.69, 1.78, 1.87, 1.96, 2.05, 2.15, 2.26, 2.37, 2.49,
	2.61, 2.74, 2.87, 3.01, 3.16, 3.32, 3.48, 3.65, 3.83, 4.02, 4.22, 4.42, 4.64, 4.87, 5.11, 5.36, 5.62, 5.90, 6.19, 6.49,
	6.81, 7.15, 7.50, 7.87, 8.25, 8.66, 9.09, 9.53
};
const float e96[] =
{
	1.00, 1.02, 1.05, 1.07, 1.10, 1.13, 1.15, 1.18, 1.21, 1.24, 1.27, 1.30, 1.33, 1.37, 1.40, 1.43, 1.47, 1.50, 1.54, 1.58,
	1.62, 1.65, 1.69, 1.74, 1.78, 1.82, 1.87, 1.91, 1.96, 2.00, 2.05, 2.10, 2.15, 2.21, 2.26, 2.32, 2.37, 2.43, 2.49, 2.55,
	2.61, 2.67, 2.74, 2.80, 2.87, 2.94, 3.01, 3.09, 3.16, 3.24, 3.32, 3.40, 3.48, 3.57, 3.65, 3.74, 3.83, 3.92, 4.02, 4.12,
	4.22, 4.32, 4.42, 4.53, 4.64, 4.75, 4.87, 4.99, 5.11, 5.23, 5.36, 5.49, 5.62, 5.76, 5.90, 6.04, 6.19, 6.34, 6.49, 6.65,
	6.81, 6.98, 7.15, 7.32, 7.50, 7.68, 7.87, 8.06, 8.25, 8.45, 8.66, 8.87, 9.09, 9.31, 9.53, 9.76
};

void AddStandardValues(set<float> *db, const float *values, int n)
{
	for (int i = 0; i < sizeof(e12) / sizeof(*e12); i++)
	{
		db->emplace(values[i]);
		db->emplace(values[i] * 1000);
		db->emplace(values[i] * 10000);
		db->emplace(values[i] * 100000);
		db->emplace(values[i] * 1000000);
	}
}

int main(int argc, char* argv[])
{
	set<float> db;
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
		if (s == "--e6")
			AddStandardValues(&db, e6, sizeof(e6) / sizeof(*e6));
		else if (s == "--e12")
			AddStandardValues(&db, e12, sizeof(e12) / sizeof(*e12));
		else if (s == "--e24")
			AddStandardValues(&db, e24, sizeof(e24) / sizeof(*e24));
		else if (s == "--e48")
			AddStandardValues(&db, e48, sizeof(e48) / sizeof(*e48));
		else if (s == "--e96")
			AddStandardValues(&db, e96, sizeof(e96) / sizeof(*e96));
		else if(s == "--database")
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
		if (db.size() == 0)
		{
			fprintf(stderr, "No component database specified, use --database file.txt or one of the --e arguments to use standard values\n");
			return 1;
		}
	}
	else
	{
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
			db.emplace(ParseResistance(line));
		}
		fclose(fp);
	}

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
	printf("Usage: vdcalc (database) (goal) (constraints)\n");
	printf("\n");
	printf("Database of legal component values (must specify one or more):\n");
	printf("    --database file.txt: read parts from an inventory file containing one resistor value per line\n");
	printf("    --e6, --e12, --e24, --e48, --e96: use standard 20, 10, 5, 2, or 1% resistor values\n");
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
