/* tol_bvmat_bsr_run_h: BVMat Bayesian Sparse Regression 
                              running parser
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

const char* fName = fileName.c_str();
//BOOST_SPIRIT_DEBUG_NODE(bsr);
ifstream in(fName);
if(!in)
{
  Error(BSR() + BText("Could not open input file: ") + 
        fName + "\n");
  return(-1);
}
bsr.fileName = fileName;
bsr.fileSize = GetFileSize(fName);
bsr.file = &in;

in.unsetf(ios::skipws); //  Turn of white space skipping on the stream
skip_grammar skip;
typedef position_iterator< file_iterator<char> > iterator_t; 
file_iterator<char> fiter(fileName); 
iterator_t begin(fiter, fiter.make_end(), fName);
iterator_t end;  
Std(BSR()+"Parsing BSR file "+fName+" with "+(int)bsr.fileSize+" bytes\n");
parse_info<iterator_t> result = parse(begin, end, bsr, skip);
if(!result.full && !bsr.endFound_)
{
  BText msg = BSR() + BText(fName) + " Fails Parsing\n"+url_parse_bsr();
  BText desc;
  for (int i = 0; i < 1000; i++)
  {
    if (result.stop == end) break;
    desc += *result.stop++;
  }
  if(!desc.HasName()) { desc = "Unexpectend end of file"; }
  Error(msg+"\nProblem description:'"+desc+"'");
  return(-2);
} 
