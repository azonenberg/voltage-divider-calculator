# Introduction

This is a simple console utility for solving a problem every electrical engineer faces frequently: choosing parts for
a voltage divider.

This tool takes a series of command line arguments specifying the desired division ratio and various constraints (for
example, total resistance of the divider must be at least X but no more than Y, or the lower resistor must have a fixed
value, a text file listing the available resistor values available in inventory, and outputs suggested component values
and error bounds.
