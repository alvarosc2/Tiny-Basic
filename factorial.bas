10 PRINT "Calculates the factorial of a number"
20 PRINT "Introduce a whole number: "
30 INPUT A
40 PRINT "The factorial of ", A, "is:"
50 GOSUB 80
60 PRINT B
70 END
80 LET B = 1
90 LET B = B * A
100 LET A = A - 1
110 IF A >= 1 THEN GOTO 90
120 RETURN