[ subtraction.bf - Subtract two single digit numbers where n1 >= n2.
  Copyright (C) 2021 Robert Coffey ]

,>,                             Read two bytes into cells #0 and #1
[-<->]                          Subtract cell #1 from cell #0
++++[>++++[<<+++>>-]<-]<.       Add 48 to cell #0 and write it
>++++++++++.                    Write a newline
