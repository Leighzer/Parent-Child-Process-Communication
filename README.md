# Parent-Child-Process-Communication
This was my first assignment in my Operating Systems class. This is a C program where the parent process spawns child processes and communicates to them sections of a file of integers to sum. The children sum their portion of the file, they then pipe their results back. The parent then sums the child sums, and prints the grand total.


I forget the exact feedback of the project, but my original findings in the word doc ReadMe were incorrect. I believe it is because I have the file read in a single stream and include that in the timings. There was feedback that the entire file should have been read in and ingested for the child processes to work on. They said that most of the time being taken up is actually by the parent I/O. I was timing how the long entire program was running for, where as I should have started timing when the child summations started. This would've given a much better view in how more parallezation affects my speed up.
