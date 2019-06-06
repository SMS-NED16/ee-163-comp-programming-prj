# EE-163 Project - K-Electric Electricity Bill Calculation Program

## A simple, command-line based electricity bill calculation program with file I/O, basic authentication, and visualization. 

## More Info
- Built as an end of semester project for EE-163 Computers and Programming, an introductory programming course as part of BE-Electrical Engineering at NEDUET.
- This was one of my first full-fledged academic programming projects. Needless to say, it is very basic. 
- This project was intended to be a simple, command-line version of an online customer for K-Electric, hence the authentication and basic DB functionality.
- KE classifies its consumers into one of three categories - residential, commercial, or industrial.
   Bill calculation uses different tariffs/slabs depending on the user's category.
- For the purposes of this programme, there are three users - one belonging to each category. 
- The program implements a basic login system for all users. Details for these users are as follows:
      Category		Category Letter		Username	Password
   Residential		              A		    A163	   1234A
    Commercial			            B	      B163     1234B
    Industrial                  C		    C163	   1234C
- For file input, ensure that files A163.txt, B163.txt, and C163.txt are in the same directory as main.cpp
- A163/B163/C163 contain sample bill data for users A163, B163, and C163 respectively. 
- This data is stored in the order:
   (load in kiloWatts)/(month number)/(year)/(units)/(payment flag)/(three phase flag)/(supply voltage)
