/* tolcon2.cpp: Interactive Console for GNU/TOL language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
   Daniel Rus <daniel@uponeits.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

//------------------------------------------------------------
// screen control
//------------------------------------------------------------
#include <ncurses.h>
#include <string.h>

//------------------------------------------------------------
// tol related includes
//------------------------------------------------------------
#include <tol/tol_blanguag.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bmoninfo.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bmatimp.h>
#include <tol/tol_delay_init.h>

#define MAX_STMTS    500
#define LENGTH_STMT  100

#define WIDTH_WIN_CALENDAR 50
#define HEIGHT_WIN_CALENDAR 10

//------------------------------------------------------------
// type definitions
struct statement {
    int edited; // true when stmt field has been edited
    char stmt[LENGTH_STMT];
};
typedef struct statement statement;

struct TMS_ClientData {
    BUserTimeSet *uts;
    BDate current;
    
    void init(BUserTimeSet *t) {
	if (uts = t) {
	    uts->IncNRefs();
	    start(BDate::DefaultFirst());
	}
    }

    void destroy () {
	if (uts) { uts->DecNRefs(); uts = NULL;	}
    }

    void start (BDate & d) {
	if (uts) current = uts->FirstNoLess(d);
    }
    
    void start (char * txt) {
	if (txt) current.PutDate(txt);
	else     current = BDate::DefaultFirst();
	if (uts) current = uts->FirstNoLess(current);
    }

    BDate & getDate () { return current; }

    BDate & next () {
	if (uts) current = uts->Successor(current);
	return   current;
    }

    BDate & previous () {
	if (uts) current = uts->Predecessor( current );
	return current;
    }
    
    int hasValue()           { return current.HasValue();}
    int getYear()      const { return current.Year();    }
    int getMonth()     const { return current.Month();   }
    int getDay()       const { return current.Day();     }
    int getHour()      const { return current.Hour();    }
    int getMinute()    const { return current.Minute();  }
    double getSecond() const { return current.Second();  }
    int contain(BDate & d)   { return uts->Contain(d);   }
};

//------------------------------------------------------------
// statics variables declaration
static statement stmts[MAX_STMTS];
static int stmtInd = -1;
static statement curStmt;
WINDOW* mainScr;
static bool insertMode = true;
static TMS_ClientData tmsClDat;
static int datesPerWin = HEIGHT_WIN_CALENDAR;

// Welcome message printed at start
static char welcomeMsg[] = 
"Welcome to TolCon, the Tol Interactive Console\n\n"
" Type:  \\copyright for distribution terms\n"
"        \\h         for help with Tol commands\n"
"        \\?         for help on internal slash commands\n"
"        \\q         to quit\n\n";

// Help message printed with \help
static char helpMsg[] =
"\n-- slash commands list --\n\n"
"Command   Description\n"
"-------   -------------------------------------------\n"
"\\?        Help on internal slash commands\n"
"\\h        Help with Tol commands\n"
"\\show <Type> <VarName>\n"
"         <Type> is anyone of each Tol types\n"
"         <VarName> is the name of variable to show\n"
"\\datesperwin <NewValue>\n"
"         Set the number of time instants shown in a TimeSet window.\n"
"         <NewValue> is the new value for this feature, default value is 10\n"
"\\clear    Clear screen\n"
"\\q        To quit (or type 'exit', 'quit' or 'end')\n\n";

//------------------------------------------------------------
// global variables
int startx = 0;
int starty = 0;

//------------------------------------------------------------
// functions declaration

//! Used as a print function BHciWriter, a type defined in BOut class
void winWriter (const BText& txt);
//! begin initialization proccess of Tol types
void tolInitialize (int init);
//! Used to Load the TolInitLibrary
char initTolFile (const BText& txt);
//! return true when \a sentence given as argument is "exit", "end" or "quit"
bool isExit (BText& sentence);
//! main method of the dialog created between user and Tol
void consoleDialog ();
//! method for manipulate command line
char* readStmt ();
//! proccess tol console internal command -> commands that begins with "\"
void internalCommand (const char *cmd);
//! process "\show" slash internal command, that show variables values
void showCommand (char *cmd);
//! process "\set" slash internal command, used to set internal tolcon vars
void setCommand (char *cmd);
//! creates an ncurses window for TimeSet representation
void calendarWindow ();
//! fill window created with calendarWindow, with values of a TimeSet
void printCalendarWindow (WINDOW *calWin, BDate firstDate);
//! erase data from calendar window, used to continue adding new data 
void cleanCalendarWindow (WINDOW *calWin);
//------------------------------------------------------------
// functions definition

void winWriter (const BText& txt) {
    printw(txt.String());
    refresh();
}

char initTolFile (const BText& txt) {
    BText vn = BText("")+VERSION_MAJOR+"."+VERSION_MINOR+"."+VERSION_RELEASE;
    BText rot1 = BText("Time Oriented Language, v") + vn + "\n";
    BText rot2 = BText("Tol Interactive Console\n");
    BText rot3 = BText("Loading Tol Init Library ") + txt + "\n";

    printw(rot1.Buffer());
    printw(rot2.Buffer());
    printw(rot3.Buffer());
    refresh();

    BSyntaxObject* initTol =
	GraSet()->Evaluate(Out()+"Set InitTol = Include(\""+txt+"\")");
}

void tolInitialize (int init) {
    BText::PutFormatBReal("%lf");
    BText::PutFormatBInt("%ld");
    BGrammar::PutLast(GraSet());
    BOut::PutAllTerm(BFALSE);
    BOut::PutStdHci(BTRUE);

    // loading tol types support
    InitGrammars();

    // loading TolInitLibrary
    if (init) {
	BText initpath = BSys::GetEnv( "TOL_LIBRARY" );
	
	if ( initpath != "" ) initpath.Replace( '\\', '/' );
	else initpath = "./initlibrary";
	
	BDir initTolPath =  initpath + "/_inittol.tol";
	
	if(initTolPath.Exist()) initTolFile(initTolPath.Name());
	else {
	    initpath = TOLINIT_LIB_DIR;
	    initTolPath =  initpath + "/_inittol.tol";
	    if(initTolPath.Exist()) initTolFile(initTolPath.Name());
	}
    }

    BOut::PutHciWriter(winWriter);
    refresh();
}

bool isExit(BText& sentence) {
    if(sentence.Length()>=4) {
	char *st = sentence.Buffer();
	if     (!strncmp(st, "exit", 4)) return true;
	else if(!strncmp(st, "END",  3)) return true;
	else if(!strncmp(st, "end",  3)) return true;
	else if(!strncmp(st, "quit", 4)) return true;
	else return false;
    } else return false;
}

char* readStmt () {
    static int lastIndex;

    stmtInd++;
    if (stmtInd==MAX_STMTS) stmtInd=0;
    curStmt.stmt[0] = '\0';
    lastIndex = stmtInd;

    int x, y, curPos=0, endPos=0;
    int ch = getch();
    getyx(mainScr, y, x);

    while(ch!=10) {
	switch(ch) {

	    case KEY_LEFT:      // move cursor to left side
		getyx(mainScr, y, x);
		if(x>5) { move(y, --x); curPos--; }
		break;

	    case KEY_RIGHT:     // move cursor to right side
		getyx(mainScr, y, x);
		if( x <= (endPos+5) ) { move(y, ++x); curPos++; }
		break;

	    case KEY_UP:        // Key Up Pressed
		curStmt.stmt[endPos]='\0';
		for(int i=endPos; i>=0; i--) mvdelch(y, i+5);

		strcpy(stmts[lastIndex].stmt, curStmt.stmt);
		stmts[lastIndex].edited=1;

		lastIndex = ((lastIndex==0) ? (MAX_STMTS-1) : lastIndex-1);
		while((!stmts[lastIndex].edited) && (lastIndex!=stmtInd))
		    lastIndex = ((lastIndex==0) ? (MAX_STMTS-1) : lastIndex-1);
		
		strcpy(curStmt.stmt, stmts[lastIndex].stmt);
		
		curPos = endPos = strlen(curStmt.stmt);
		mvinsstr(y, 5, curStmt.stmt);
		move(y, curPos+5);
		break;

	    case KEY_DOWN:      // Key Down Pressed
		curStmt.stmt[endPos]='\0';
		for(int i=endPos; i>=0; i--) mvdelch(y, i+5);

		strcpy(stmts[lastIndex].stmt, curStmt.stmt);
		stmts[lastIndex].edited=1;

		lastIndex = ((lastIndex==(MAX_STMTS-1)) ? 0 : lastIndex+1);
		while((!stmts[lastIndex].edited) && (lastIndex!=stmtInd))
		    lastIndex = ((lastIndex==(MAX_STMTS-1)) ? 0 : lastIndex+1);

		strcpy(curStmt.stmt, stmts[lastIndex].stmt);

		curPos = endPos = strlen(curStmt.stmt);
		mvinsstr(y, 5, curStmt.stmt);
		move(y, curPos+5);
		break;

	    case KEY_IC:        // Insert Key Pressed
		insertMode = !insertMode;
		break;

	    case KEY_HOME:      // Key Home Pressed
		curPos = 0; move(y, 5);
		break;

	    case KEY_END:       // Key End Pressed
		curPos = endPos; move(y, curPos+5);
		break;

	    case KEY_BACKSPACE: // delete character before cursor
		if(curPos>0) {
		    for(int i=curPos; i<=endPos; i++)
			curStmt.stmt[i-1] = curStmt.stmt[i];
		    curPos--; endPos--;
		    mvdelch(y, (curPos+5));
		    move(y, (curPos+5));
		}
		break;

	    case KEY_DC:
		for(int i=curPos; i<endPos; i++)
		    curStmt.stmt[i] = curStmt.stmt[i+1];
		endPos--;
		mvdelch(y, (curPos+5));
		move(y, (curPos+5));
		break;

	    default:            // add a character...
		if(curPos==endPos) { // 1- ...at end line
		    addch(ch);
		    curStmt.stmt[curPos] = ch;
		    endPos = ++curPos;
		} else {      // 2- ... before the end line
		    if(insertMode) {  // insert
			for(int i=endPos; i>curPos; i--)
			    curStmt.stmt[i] = curStmt.stmt[i-1];
			curStmt.stmt[curPos] = ch;
			mvinsch(y, (curPos+5), ch);
			curPos++; endPos++;
			move(y, (curPos+5));
		    } else {          // replace
			curStmt.stmt[curPos] = ch;
			mvdelch(y, (curPos+5));
			mvinsch(y, (curPos+5), ch);
			curPos++;
			move(y, (curPos+5));
		    }
		}

	}
	refresh();
	ch = getch(); 
    }
    curStmt.stmt[curPos] = '\0';
    strcpy(stmts[stmtInd].stmt, curStmt.stmt);
    stmts[stmtInd].edited = 1;
    return stmts[stmtInd].stmt;
}

void consoleDialog () {
    char *curStmt, lastSym;
    int openBlk=0; 
    BText sentence="";
    BList* stack = NIL;
    BGrammar::PutLast(GraReal());
    printw("\ntol> ");

    curStmt = readStmt();
    sentence += BText(curStmt) + " ";
    printw("\n");

    while(!isExit(sentence)) {
	for(int i=0; curStmt[i]; i++) { // counts open and close symbols
	    if((curStmt[i]=='(') || (curStmt[i]=='[') || (curStmt[i]=='{')) 
		openBlk++;
	    if((curStmt[i]==')') || (curStmt[i]==']') || (curStmt[i]=='}')) 
		openBlk--;
	    if((curStmt[i]!='\n') && (curStmt[i]!='\t') && (curStmt[i]!=' ')) 
		lastSym=curStmt[i];
	}

	if(curStmt[0]=='\\') { // tol console internal command
	    internalCommand(curStmt);
	    sentence = "";
	    printw("tol> ");
	} else if(!openBlk && lastSym==';') { // execute sentence
	    BSyntaxObject* result = Evaluate(sentence);
	    if(result) {
		Std(result->Dump()); Std("\n");
		BGrammar::PutLast(result->Grammar());
		if(!(result->Name().HasName())) { DESTROY(result); }
		else { stack = Cons(result, stack); }
	    }
	    sentence = "";
	    printw("tol> ");
	} else {
	    printw("---> ");
	}

	curStmt = readStmt();
	sentence += BText(curStmt) + " ";
	printw("\n");
    }
}

void internalCommand (const char *cmd) {
    char *command = new char[strlen(cmd)];
    strcpy(command, cmd);

    if     (!strncmp(command, "\\?", 2)) printw(helpMsg);
    else if(!strncmp(command, "\\show", 5)) {
	showCommand(command);
    }else if(!strncmp(command, "\\set", 4)) {
	setCommand(command);
    }else if(!strncmp(command, "\\clear", 6)) {
	erase(); refresh();
    }else if(!strncmp(command, "\\datesperwin", 12)) {
	command = strtok(command, " ");
	command = strtok('\0', " ");
	datesPerWin = atoi(command);
	printw("stablished\n");
    }else if(!strncmp(command, "\\q", 6)) {
	endwin(); 
	exit(1);
    } else {
	printw("Error: ");
	printw(command);
	printw(" - Internal command not found\n");
    }
}

void setCommand (char *cmd) {
}

void showCommand (char *cmd) {
    BGrammar *grammar;
    
    cmd = strtok(cmd, " ");
    cmd = strtok('\0', " ");
    
    if (!strcmp(cmd, "Anything")) {
	grammar =  GraAnything();
	printw("Access to \"Anything\" type still not implemented\n");
    } else if (!strcmp(cmd, "Code")) {
	grammar = GraCode();
	printw("Access to \"Code\" type still not implemented\n");
    } else if (!strcmp(cmd, "Date")) {
	grammar = GraDate();
	cmd = strtok('\0', " ");
	BUserDate *buDate = (BUserDate *)(grammar->FindVariable(cmd));
	if (buDate) {
	    printw((buDate->Dump()).String()); 
	    printw("\n");
	} else {
	    printw("Error: variable \""); printw(cmd);
	    printw("\" not found\n");
	    return;
	}
    } else if (!strcmp(cmd, "Polyn")) {
	grammar = GraPolyn();
	printw("Access to \"Polyn\" type still not implemented\n");
    } else if (!strcmp(cmd, "Ratio")) {
	grammar = GraRatio();
	cmd = strtok('\0', " ");
	BUserRat *buRat = (BUserRat *)(grammar->FindVariable(cmd));
	if (buRat) {
	    printw((buRat->Dump()).String()); 
	    printw("\n");
	} else {
	    printw("Error: variable \""); printw(cmd);
	    printw("\" not found\n");
	    return;
	}
    } else if (!strcmp(cmd, "Real")) {
	grammar = GraReal();
	cmd = strtok('\0', " ");
	BUserDat *buDat = (BUserDat *)(grammar->FindVariable(cmd));
	if (buDat) {
	    printw((buDat->Dump()).String()); 
	    printw("\n");
	} else {
	    printw("Error: variable \""); printw(cmd);
	    printw("\" not found\n");
	    return;
	}
    } else if (!strcmp(cmd, "Complex")) {
	grammar = GraComplex();
	printw("Access to \"Complex\" type still not implemented\n");
    } else if (!strcmp(cmd, "Matrix")) {
	grammar = GraMatrix();
	printw("Access to \"Matrix\" type still not implemented\n");
    } else if (!strcmp(cmd, "Set")) {
	grammar = GraSet();
	printw("Access to \"Set\" type still not implemented\n");
    } else if (!strcmp(cmd, "Text")) {
	grammar = GraText();
	printw("Access to \"Text\" type still not implemented\n");
    } else if (!strcmp(cmd, "TimeSet")) { 
	grammar = GraTimeSet();
	cmd = strtok('\0', " ");
	BUserTimeSet *buTimeSet = (BUserTimeSet *)(grammar->FindVariable(cmd));
	tmsClDat.init(buTimeSet);
	calendarWindow();
    } else if (!strcmp(cmd, "Serie")) {
	grammar = GraSerie();
	printw("Access to \"Serie\" type still not implemented\n");
    } else {
	printw("Error: type \""); printw(cmd);
	printw("\" not found\n");
	return;
    }

    return;
}

void calendarWindow () {
    WINDOW *calWin;
    int ymax, xmax;
    int highlight = 1;
    int choice = 0;
    int c;

    char *choices[] = { "","","Exit" };

    getmaxyx(stdscr, ymax, xmax);
    startx = (xmax - WIDTH_WIN_CALENDAR) / 2;
    starty = (ymax - (datesPerWin+6)) / 2;

    calWin = newwin((datesPerWin+6), 
		    WIDTH_WIN_CALENDAR, 
		    starty, startx);
    keypad(calWin, TRUE);
    refresh();

    BDate firstDate(tmsClDat.getDate());
    printCalendarWindow(calWin, tmsClDat.getDate());

    c = wgetch(calWin);
    while(c!=10) {
	switch(c) {
	    case KEY_UP:
		cleanCalendarWindow(calWin);
		tmsClDat.start(firstDate);
		tmsClDat.previous();
		firstDate = tmsClDat.getDate();
		printCalendarWindow(calWin, firstDate);
		break;
	    case KEY_DOWN:
		cleanCalendarWindow(calWin);
		tmsClDat.start(firstDate);
		tmsClDat.next();
		firstDate = tmsClDat.getDate();
		printCalendarWindow(calWin, firstDate);
		break;
	    case KEY_PPAGE:
		cleanCalendarWindow(calWin);
		tmsClDat.start(firstDate);
		for(int i=0; i<datesPerWin; i++) tmsClDat.previous();
		firstDate = tmsClDat.getDate();
		printCalendarWindow(calWin, firstDate);
		break;
	    case KEY_NPAGE:
		cleanCalendarWindow(calWin);
		tmsClDat.start(tmsClDat.next());
		firstDate = tmsClDat.getDate();
		printCalendarWindow(calWin, firstDate);
		break;
	}
	c = wgetch(calWin);
    }
    werase(calWin);
    wrefresh(calWin);
    delwin(calWin);
    refresh();
}

void printCalendarWindow (WINDOW *calWin, BDate date) {
    box(calWin, 0,0);
    mvwprintw(calWin, 1, 1, "%s", ((BDate::Begin()).GetText()).String());
    mvwprintw(calWin, 2, 1, "[ ... ]");

    for(int i=1; i<=datesPerWin; i++) {
	mvwprintw(calWin, i+2, 3, "%s, ", 
		  BDateFormat::LongWeekDay(date.WeekDay()));
	mvwprintw(calWin, i+2, 15, "%2d. ", date.Day());
	mvwprintw(calWin, i+2, 19, "%s, ", 
		  BDateFormat::LongMonth(date.Month()));
	mvwprintw(calWin, i+2, 32, "%4d   ", date.Year());
	mvwprintw(calWin, i+2, 39, "%02d:%02d:%02d", 
		  date.Hour(), date.Minute(), date.Second());
	
	if(i!=datesPerWin) date = tmsClDat.next();
    }

    mvwprintw(calWin, (datesPerWin+3), 1, "[ ... ]");
    mvwprintw(calWin, (datesPerWin+4), 1, "%s",
	      ((BDate::End()).GetText().String()));  

    wrefresh(calWin);
}

void cleanCalendarWindow (WINDOW *calWin) {
    char emptyLine[] = "                                              ";
    for(int i=1; i<=datesPerWin; i++)
	mvwprintw(calWin, i+2, 3, "%s, ", emptyLine);
}

int main (int argc, char **argv) {
    // initialize buffer of sentence
    for (int i=0; i<MAX_STMTS; i++) {
	for (int j=0; j<LENGTH_STMT; j++)
	    stmts[i].stmt[j] = ' ';
	stmts[i].stmt[0] = '\0';
	stmts[i].edited = 0;
    }
    curStmt.stmt[0]='\0';
    curStmt.edited = -1;

    // initialize ncurses standard screen
    mainScr = initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    scrollok(mainScr, 1);
    printw(welcomeMsg);

    // manage given command line options
    if (argc==2) {
	if (!strcmp(argv[1], "--noinit")) {
	    tolInitialize(0);
	}
    } else {
	tolInitialize(1);
    }

    consoleDialog();
    endwin();
    exit(1);
}
