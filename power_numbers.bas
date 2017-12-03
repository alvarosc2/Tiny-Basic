10 PRINT "Calculate the square and the cube of a series of 20 numbers"
20 PRINT "Number", "Square", "Cube"
30 LET A = 1
50 LET J = A * A
60 LET K = J * A
70 PRINT A, J, K
80 LET A = A + 1
90 IF A <= 20 THEN GOTO 50
100 END



