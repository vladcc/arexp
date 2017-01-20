# arexp
Infix arithmetic expression calculator

I recently got interested in making the computer calculate expressions written in the old-fashioned human way.
Typing 5 6 + 10 8 4 / - * 3 + instead of (5 + 6) * (10 - 8 / 4) + 3 just isn't fun. So, naturally, the first idea I got was to still
use the postfix notation and a stack machine, but convert the normal infix expression behind the scenes. I did some research and found
out that it's easier said than done - postorder traversal of a binary tree works only for fully parenthesized expressions and typing
(((((5 + 6) * ((10 - (8 / 4)))) + 3) isn't much fun either, but it's a bit better. So I figured I'd get the infix expression, 
parethesize it automatically and then parse it into the binary tree. The algorithm they used in FORTRAN I seemed a good fit - just add
the needed ) and ( around the numbers and voila! That, as awesome as it is, however, couldn't handle consecutive operators of the same 
precedence when combined with the tree. Or at least I couldn't make it do so. The next option was Dijkstra's Shunting Yard, but seemed 
like an overkill.

So I sat down to think and figured something else out. It's very simple, really: parse the expression, perform the operations in order
of precedence and replace them with their results. If you see a '(', parse the expression between the parentheses recursively (I'm much 
fun at parties!)
This way 1 + (4 - 2) * 3 becomes 1 + 2 * 3, then 1 + 6, and finally only 7 is left. Pretty straightforward.

arexp (short for arithmetic expression parser, cause when you write the stuff you get to make the cool acronyms) is my implementation of
that idea. Not to bore you with details, which are documented in the source anyway, here's the short version: it's divided in three - 
arexp.c is the front end with the main task of accepting input and passing it to errchk.c, which checks for syntax errors. If there are 
syntax errors, a (mostly) meaningful message(s) is displayed. Otherwise errchk hands it over to eval.c, which calculates the expression 
and gives you back the result. The supported operations are exponentiation, multiplication, division, addition, subtraction, and negation. 
The numbers are of type double. You can use arexp directly from the command line, calculate interactively, or script your calculations 
through input redirection. For more information run arexp -h.
