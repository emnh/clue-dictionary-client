Clue for linux is an application for doing lookups in the dictionary files from Clue Norge ASA.  The linux client itself is GPL, but the data are covered by the commercial license which you got when you bought the dictionaries from Clue
Norge ASA. It's illegal to use this program with the Clue dictionaries if you do not have a license. You are not allowed to distribute the dictionaries with this program.

It should be noted that the Windows version of Clue works perfectly fine in Wine (http://www.winehq.org/), so if you prefer the regular Windows UI instead of a console version, just go into the clue directory and run "wine clue.exe", assuming you have Wine installed.

There are 2 versions:

clue 1:
Old C version, console client.

clue 2:
New python console client.
New javascript web interface using JSON dumps of dictionaries.
New PHP web interface using SQL database (dictionary to SQL dump tool included).

Here's a blogger, referred to me by a friend, describing in Norwegian how to use clue2 in simple step-by-step instructions:
http://alexander.karlstad.be/clue-ordbok-i-linux



Another related project is iClue, at http://blog.hsorbo.no/p/iclue_01.html, which is a Clue clone for Mac, though without source code, by someone I know.