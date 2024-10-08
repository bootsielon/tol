The intermediate form is a flat list of tag/value pairs.  The tags are as follows: 

PRE {text} 
	This is usually Preformatted text, to be displayed verbatim.  It's also used for blank lines between paragraphs.  (Q: can these uses be distinguished?)  The <text> can be almost anything.  PRE is often preceded and followed by HASH, if #pre and #unpre were used. 

TCL {text} 
	The {text} is expected to be Tcl code that can be evaluated. Eventually I might format it in some special way.  It's always preceded and followed by "HASH #Tcl" and "HASH #unTcl". 

HASH {#hash line} 
	The value is a single line of text (no newline at the end) beginning with a hash ("#") character.  Such lines always yield a HASH tag and value even if they affect the parsing in some other way (e.g., #pre and #Tcl), so that we can rebuild the original markup just as it was. 

P {} 
	Begins a collection of paragraph text.  If the paragraph is bulleted, it's always followed immediately by BUL and LEADING.  If the paragraph is indented, it's always followed immediately by IND and LEADING. 

	After that, you can have any combination of TEXT, STY, LINK, BTN, and MACRO tags, followed ultimately by a /P. 

BUL n 
	Follows P, and denotes a bulleted paragraph.  "n" is an integer denoting the indent level; n >= 1 

IND n 
	Follows P, and denotes an indented paragraph.  "n" is an integer denoting the indent level; n >= 1.  Note that "BUL 1" and "IND 1" result in the same level of indentation for the text. 

LEADING {whitespace} 
	Follows BUL or IND; its value is the whitespace between the stars or colons that began the paragraph and the first character of non-whitespace text.  We save this so that we can reproduce the original markup just as it was. 

TXT {text} 
	The value is plain text.  Always appears between P and /P. 

STY style-code 
	The value is a style code, e.g.,  or .  Affects the following text in the paragraph.  All styles are cancelled at the end of the paragraph.  Always appears between P and /P. 

LINK {link text} 
	The value is a page link, not including the [ and ].  Always appears between P and /P. 

BTN {button text|command} 
	The value is the definition of a magic button, not including the [% and %].  Always appears between P and /P. 

MACRO {command} 
	The value is an embedded macro command, not including the [@ and @].  Always appears between P and /P. 


Some Observations 

The input markup is broken into chunks; each chunk represents one or more complete lines of text.  There are four kinds of chunks: 

	HASH chunks.  One line of text, to be used as a directive by the parser or the renderer.  If the hash is unrecognized, it's rendered as preformmated text. 

	PRE chunks.  Preformatted text, to be rendered as is. 

	TCL chunks.  Preformatted Tcl code, to be rendered as is, or in some appropriate fashion for Tcl code. 

	P chunks.  Paragraphs, possibly bulleted or indented, which can contain TXT, STY, LINK, BTN, and MACRO text. 
