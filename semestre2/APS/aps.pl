

/* axiomes */

%bool
bool(true).
bool(false).

%int
int(z).
int(s(Z)) :- int(Z).

%void
void(cs).

/****************** typage *********************/
type(true, bool).
type(false, bool).
type(Z, int):- int(Z).

/* typage expression logique */
type(and(X,Y), bool) :- type(X,T) , type(Y,T), T == bool, !.
type(or(X,Y), bool) :- type(X,T) , type(Y,T), T == bool, !.
type(not(X), bool) :- type(X,bool), !.
type(eq(X,Y), bool) :- type(X,T1) , type(Y,T2), T1 == T2, !.
type(lt(X,Y), bool) :- type(X,T) , type(Y,T), T == int, !.

/* typage expression arithmétique */

type(add(X,Y), int) :- type(X,T) , type(Y,T), T == int, !.
type(sub(X,Y), int) :- type(X,T) , type(Y,T), T == int, !.
type(mul(X,Y), int) :- type(X,T) , type(Y,T), T == int, !.		
type(div(X,Y), int) :- type(X,T) , type(Y,T), T == int, !.


/* typage instruction  */
:- dynamic mylist/1.
mylist([]).


env(_{}) .

saveList(Item,R):-
	mylist(X),
	append(X,[Item],R),
	assertz(mylist(R)).











