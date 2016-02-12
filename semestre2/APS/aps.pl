
/* axiomes */
%mere
mere(telnet,ssh).

%bool
bool(true).
bool(false).

%int
int(z).

%type
type(true, bool).
type(false, bool).
type(z, int).
type(and(X,Y), bool) :- type(X,T) , type(Y,T), T = bool.

type(E , T)




/* expression */
%and
and(X,Y) :- type(X,T) , type(Y,T), T = bool.

