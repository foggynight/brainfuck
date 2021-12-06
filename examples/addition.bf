[ addition.bf - Add two single digit numbers where n1 + n2 < 10.
  Copyright (C) 2021 Robert Coffey ]

,>,                             Read two bytes into cells #0 and #1
>++++[>++++[<<--->>-]<-]<       Subtract 48 from cell #1
[-<+>]<.                        Add cell #1 to cell #0 and write it
>++++++++++.                    Write a newline
