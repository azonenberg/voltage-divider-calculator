# Introduction

This is a simple console utility for solving a problem every electrical engineer faces frequently: choosing parts for
a voltage divider. Very often you are limited by parts you have on the shelf (so you can prototype right now without
waiting for an order to arrive), or have constraints imposed on component values.

This tool takes a series of command line arguments specifying the desired division ratio, a text file listing the
available resistor values available in inventory, and outputs suggested component values and error bounds. Additional
optional arguments allow you to specify constraints: for example, total resistance of the divider must be at least X
but no more than Y, or the lower resistor (R2) must have a fixed value.

# Usage

`vdcalc --database resistors.txt` (goal) (constraints)

## Goal

The goal argument specifies what the relationship of the divider resistors to each other should be. Two formats are
supported:

`--divide NNN`: specifies a division of NNN. This can be expressed mathematically as finding solutions to the equation
$ NNN = R2 / (R1 + R2) $.

For example, R1 = R2 for any resistor value in the database is a valid solution given a goal of `--divide 0.5`.

`--ratio NNN`: specifies the ratio between R1 (upper) and R2 (lower). This can be expressed mathematically as finding
solutions to the equation $R1 = NNN * R2$.

For example, R1 = 20K, R1 = 10K is a valid solution given a goal of `--ratio 2`.

## Constraints

Constraint arguments specify requirements that a valid solution must satisfy. More than one constraint may be used.

`--max-sum NNN`: the sum of R1 and R2 cannot be greater than NNN.

`--min-sum NNN`: the sum of R1 and R2 cannot be less than NNN.

`--max-r1 NNN`: the value of R1 cannot be greater than NNN.

`--min-r1 NNN`: the value of R1 cannot be less than NNN.

`--max-r2 NNN`: the value of R1 cannot be greater than NNN.

`--min-r2 NNN`: the value of R1 cannot be less than NNN.

# Database file

The database file is a list of resistor values which are available for use. This is an ASCII text file with one
resistor value per line. Values may use "k" or "M" suffixes, for example "1M" or "4.7k".
