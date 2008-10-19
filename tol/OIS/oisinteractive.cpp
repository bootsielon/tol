/* oisinteractive.cpp: main and init functions of TOL language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_oisloader.h>
#include <tol/tol_bdir.h>


BTraceInit("oisinteractive.cpp");


//--------------------------------------------------------------------
// Console interactive OIS
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  void BOisLoader::GoTo(const BText& child)
//--------------------------------------------------------------------
{
  BText aux0 = child;
  BText aux1 = aux0.ExternCompact();
  BText aux2 = isdigit(aux1[0])?aux1:BText("\"")+aux1+"\"";
  BText expr  = BText("[[")+aux2+"]]";
  BUserSet* uset = (BUserSet*)GraSet()->EvaluateExpr(expr);
  if(uset && (uset->Grammar()==GraSet()))
  {
    BSet& partial = uset->Contens();
    if(SearchOffsetInHierarchy(0,0,&partial,1)) 
    {
      Std(BText("\nOIS Hierarchy {")+curHrchyPartialPath_+"} = \n");
      ShowHierarchy("",-1,0,1,-1,stdout);
    }
  }
}

//--------------------------------------------------------------------
  bool BOisLoader::Check()
//--------------------------------------------------------------------
{
  Ensure(Open(true));
  Ensure(XMLReadHeader());
  Ensure(ReadHierarchyIndex());
  Ensure(InitReaded());
  Ensure(SearchOffsetInHierarchy(NULL));
  return(true);
}

//--------------------------------------------------------------------
  BOisLoader* BOisLoader::Open(const BText& root, const BSet* address)
//--------------------------------------------------------------------
{
  BOisLoader* ois = new BOisLoader();
  ois->SetPath(root,address);
  if(!ois->Check()) 
  {
    delete ois;
    ois = NULL;
  }
  return(ois); 
}

//--------------------------------------------------------------------
  bool BOisLoader::Command(const BText& command_)
//--------------------------------------------------------------------
{
  BText command = ToUpper(command_);
  if(command=="CLOSE") 
  {
    Close();
    return(false);
  } 
  else if(command.BeginWith("GOTO "))
  {
    GoTo(command.String()+5);
  }
  return(true);
}

//--------------------------------------------------------------------
  void BOisLoader::Interactive()
//--------------------------------------------------------------------
{
  Std("\n-----------------------------------------------------------------");
  Std(I2("\nOIS Stores Interactive Viewer.",
         "\nVisualizador Interactivo de almacenes OIS."));
  Std("\n-----------------------------------------------------------------");
  Std(I2("\nType HELP and return to know more about this environment.",
         "\nTeclee HELP para saber más de este entorno."));
  BText txt;
  while(ToUpper(txt)!="QUIT")
  {
  //Std(DialogInfo());
    Std(Out() << "\nDIR " << BDir::GetCurrent().Name());
    Std(Out() << "\nOIS>");
    GetLine(cin, txt);
    BText command = Compact(ToUpper(txt));
    if(command.BeginWith("CD "))
    { 
      BDir::ChangeCurrent(Replace(txt,"\\","/").String()+3);
      BSys::System("dir");
    }
    else if(command.BeginWith("DIR ")||command.BeginWith("LS "))
    { 
      BSys::System(txt.String());
    }
    else if(command.BeginWith("OPEN "))
    { 
      BText root(txt.String()+5,txt.Length()-5);
      BOisLoader* ois = BOisLoader::Open(root);
      if(ois)
      {
        Std("OIS has been opened successfully.");
        Std(BText("\nOIS Hierarchy {} = \n"));
        ois->ShowHierarchy("",0,0,1,-1,stdout);
        do
        {
          Std(Out() << "\nOIS>");
          GetLine(cin, txt);
        }
        while(ois->Command(txt));
      }
      else
      {
        Std("Cannot open OIS.");
      }
    }
    else if(command=="HELP")
    {
      Std(
      "ls/dir [args] : shows current directory as operative system command\n"
      "cd [args]     : change current directory as operative system command and shows it\n"
      "open ois_dir  : opens an OIS store and goes to root position\n"
      "close         : closes current OIS store\n");
    }
  }
}

