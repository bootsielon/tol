/* cluster.cpp: Set grammars functions of GNU/TOL language.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>

#include "../../contrib/clusterlib/cluster.h" /* The C Clustering Library */

BTraceInit("clustergra.cpp");

BBool input_int_param(BSyntaxObject * arg, int & i)
{
  BDat* real_dat;

  real_dat = &Dat(arg);
  i = int(real_dat->Value());
  if (!real_dat->IsKnown() || i<=0)
    return false;
  return true;
}

/*
* Find the first occurrence of find in s.
*/
char *strcasestr(register char *s, const register char *find)
{
  register char c, sc;
  register size_t len;

  if ((c = *find++) != 0) {
    len = strlen(find);
    do {
      do {
        if ((sc = *s++) == 0)
          return (NULL);
      } while (toupper(sc) != toupper(c));
    } while (strncasecmp(s, find, len) != 0);
    s--;
  }
  return ((char *) s);
}

BBool input_text_param(BSyntaxObject * arg, char & ch, char * options)
{
  char * pos;

  if (arg) {
    BText& text_dat = Text(arg);
    if (text_dat.Length()==1 && (pos=strcasestr(options, text_dat.String()))) {
      ch = options[pos - options];
      return true;
    }
    else
      return false;
  }
  return true;
}

BBool check_matrix_param(BSyntaxObject * arg, int & rows, int & cols)
{
  BMat* mat_dat;

  mat_dat = &Mat(arg);
  rows = mat_dat->Rows();
  cols = mat_dat->Columns();
  if (rows<=0 || cols<=0) {
    return false;
  }
  return true;
}

BBool input_parameters(BTmpObject<BGraContens<BSet> > * f, bool h,
                      int& rows_data, int& cols_data,
                      int& kclusters, int& npass,
                      char& method, char methods[], char& dist,
                      double ** vect_c_weight,
                      double *** mat_c_data, int *** mat_c_mask)
{
  char distances[] = "ebcauxskh";
    // e - Euclidean
    // b - City-block
    // c - Pearson correlation coeficient
    // a - Absolute value of Pearson correlation coeficient
    // u - Uncentered Pearson correlation
    // x - Absolute uncentered Pearson correlation
    // s - Spearman큦 rank correlation
    // k - Kendall큦 
    // h - Shape 
  int rows, cols;
  BMat* mat_dat;
  int i, j, idxarg = 1;
  BSyntaxObject * arg,
                * arg1;

  arg1 = arg = (BSyntaxObject*)(f->Arg(idxarg++));
                               // Checking of Matrix value (data)
  if (!check_matrix_param(arg, rows_data, cols_data)) {
    Error("Invalid data matrix: number of rows and columns must be greater than zero");
    return false;
  }
  arg = (BSyntaxObject*)(f->Arg(idxarg++));
                                // Checking of Real values
  if (!input_int_param(arg, kclusters)) {
    Error("Argument k must be a positive number");
    return false;
  }
  if (kclusters>rows_data) {
    Error("Argument k can큧 be greater than number of data matrix큦 rows");
    return false;
  }
  if (!h) {        //  not hierarchical
    arg = (BSyntaxObject*)(f->Arg(idxarg++));
    if (!input_int_param(arg, npass)) {
      Error("Argument n must be a positive number");
      return false;
    }
  }
                                // Checking of optional Text values
  arg = (BSyntaxObject*)(f->Arg(idxarg++));
  if (!input_text_param(arg, method, methods)) {
    Error("Method not admited");
    return false;
  }
  arg = (BSyntaxObject*)(f->Arg(idxarg++));
  if (!h)        //  not hierarchical
    dist = method=='m' ? 'b' : 'e';
  if (!input_text_param(arg, dist, distances)) {
    Error("Admited distances are: \"e\", \"b\", \"c\", \"a\", \"u\", \"x\", \"s\", \"k\" and \"h\"");
    return false;
  }
  arg = (BSyntaxObject*)(f->Arg(idxarg++));
  if (arg) {                  // optional weights vector
    if (!check_matrix_param(arg, rows, cols)) {
      Error("Invalid weight matrix: number of rows and columns must be greater than zero");
      return false;
    }
    if (rows!=1) {
      Error("Invalid weight matrix: number of rows must be one");
      return false;
    }
    if (cols!=cols_data) {
      Error("Invalid weight matrix: number of columns must be equal than columns of data matrix");
      return false;
    }
                // Conversion from Matrix to double* (weights vector)
    mat_dat = &Mat(arg);
    *vect_c_weight = new double[cols_data];
    for (j=0; j<cols_data; j++)
      if ((*mat_dat)(0,j).IsKnown())
         (*vect_c_weight)[j] = (*mat_dat)(0,j).Value();
      else
         (*vect_c_weight)[j] = 1.0;
  }
  else {
    *vect_c_weight = new double[cols_data];
    for (j=0; j<cols_data; j++)    
      (*vect_c_weight)[j] = 1.0;
  }
                     // Conversion from Matrix to double**
  mat_dat = &Mat(arg1);       // data matrix & mask matrix
  *mat_c_data = new double*[rows_data];
  *mat_c_mask = new int*[rows_data];
  for (i=0; i<rows_data; i++) {
    (*mat_c_data)[i] = new double[cols_data];
    (*mat_c_mask)[i] = new int[cols_data];
    for (j=0; j<cols_data; j++)    
      if ((*mat_c_mask)[i][j] = (*mat_dat)(i,j).IsKnown())
        (*mat_c_data)[i][j] = (*mat_dat)(i,j).Value();
      else
        (*mat_c_data)[i][j] = 0.0;
  }
  return true;
}

void make_output(BSet& result, double c_error, int ifound, int kclusters,
                               int rows_data, int cols_data, double ** centroids_data,
                               int ** centroids_mask, int * vect_clusterid,
                               double ** mat_c_data, int ** mat_c_mask)
{
  BUserDat * real_error,            // output data
           * real_ifound;
  BUserMat * vect_assignments,
           * mat_centroids,
           * mat_one_element;
  BMat* mat_res;
  BUserSet * set_res;
  char clustername[20],
       elementname[20];
  BSet set_of_clusters,
       one_cluster_set;
  int i, j, k;
                                     // Output conversion to Real
  real_error = new BContensDat("AverageDistortion", c_error,
                            "Average distortion");
  real_ifound = new BContensDat("TimesSolutionFound", ifound,
                             "Times solution found");
                                    // from double** to Matrix
  mat_centroids = new BContensMat("Centroids", BMat(),
					          "Centroids");
  mat_res = &(mat_centroids->Contens());
  mat_res->Alloc(kclusters, cols_data);
  for (i=0; i<kclusters; i++)
    for (j=0; j<cols_data; j++)
      if (centroids_mask[i][j])
        (*mat_res)(i,j) = centroids_data[i][j];
      else
        (*mat_res)(i,j).PutKnown(BUNKNOWN);
  vect_assignments = new BContensMat("Assignments", BMat(),
					              "Assignments");
  mat_res = &(vect_assignments->Contens());
  mat_res->Alloc(rows_data, 1);
  for (i=0; i<rows_data; i++)
    (*mat_res)(i,0) = vect_clusterid[i]+1;
                                 // set of clusters
  for (k=0; k<kclusters; ++k) {
    for (i=0; i<rows_data; ++i)
      if (vect_clusterid[i] == k) {
        sprintf(elementname, "element %d", i+1);
        mat_one_element = new BContensMat(elementname, BMat(),
					                   elementname);
        mat_res = &(mat_one_element->Contens());
        mat_res->Alloc(1, cols_data);
        for (j=0; j<cols_data; j++)
          if (mat_c_mask[i][j])
            (*mat_res)(0,j) = mat_c_data[i][j];
          else
            (*mat_res)(0,j).PutKnown(BUNKNOWN);
        one_cluster_set.AddElement(mat_one_element);
      }
    sprintf(clustername, "cluster %d", k+1);
    set_of_clusters.AddElement(new BContensSet(clustername,one_cluster_set,
                                            clustername));
    one_cluster_set.Delete();
  }
  set_res = new BContensSet("SetOfClusters", set_of_clusters,
                         "Set of clusters");
                            // Result set construction
  result.RobElement(Cons(real_error,
                        Cons(real_ifound,
                            Cons(vect_assignments,
                                Cons(mat_centroids,
                                    Cons(set_res,
                                         NIL))))));
}

void del_input_data(int rows_data, double ** mat_c_data, int ** mat_c_mask,
                    double * vect_c_weight, int * vect_clusterid)
{
  int i;

  for (i=0; i<rows_data; i++) {
    delete[] mat_c_data[i];
    delete[] mat_c_mask[i];
  }
  delete[] mat_c_data;
  delete[] mat_c_mask;
  delete[] vect_c_weight;
  if (vect_clusterid)
    delete[] vect_clusterid;
}

void del_centroids_data(int kclusters, double ** centroids_data, int ** centroids_mask)
{
  int i;

  for (i=0; i<kclusters; i++) {
    delete[] centroids_data[i];
    delete[] centroids_mask[i];
  }
  delete[] centroids_data;
  delete[] centroids_mask;
}

void del_dist_matrix(int nrows, double ** distmatrix)
{
  int i;

  if (!distmatrix)
    return;
  for (i=1; i<nrows; i++)
    free(distmatrix[i]);
  free(distmatrix);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetCluster);
DefExtOpr(1, BSetCluster, "Cluster", 3, 6,
  "Matrix Real Real Text Text Matrix",
  "(Matrix data, Real k, Real n [, Text method=\"a\", Text dist, Matrix weigth])",
  "Divides the rows of matrix 'data' into 'k' clusters such that the sum of distances over the rows to their cluster centers (centriods) is minimal."
  " Unknown values (?) are accepted.\n"
  "It큦 based on the aplication of the EM algorithm 'n' times in order to find the optimal solution.\n"
  "Optional parameters are:\n"
  "  method - specifies the method used to calculate the centroid\n"
  "    \"a\" the arithmetic mean, \"m\" the median, \"d\" the medoids\n"
  "  dist - available functions to measure similarity or distance between data\n"
  "    \"e\" Euclidean, \"b\" City-block, \"c\" Pearson correlation coeficient, \"a\" Absolute value of Pearson correlation coeficient,\n"
  "    \"u\" Uncentered Pearson correlation, \"x\" Absolute uncentered Pearson correlation, \"s\" Spearman큦 rank correlation, \"k\" Kendall큦, \"h\" Shape\n"
  "  weight - the weights that are used to calculate the distance\n\n"
  
  "Returns a set whose elements are:\n"
  "  Real AverageDistortion - The average of distances of the items to their centroid\n"
  "  Real TimesSolutionFound - How often the optimal clustering solution was found\n"
  "  Matrix Assignments - This array stores the cluster number to which each item was assigned\n"
  "  Matrix Centroids - It stores the centroids of the clusters\n"
  "  Set SetOfClusters - Each subset of this set represents one cluster with its named elements",
          BOperClassify::DataMining_);
//--------------------------------------------------------------------
void BSetCluster::CalcContens()
//--------------------------------------------------------------------
{
  int kclusters;              // given data
  int npass;
  char methods[] = "amd";
    // a - Means
    // m - Medians
    // d - Medoids
  char method = 'a';
  char dist;
  int rows_data, cols_data;
  double ** mat_c_data;
  int ** mat_c_mask;
  double * vect_c_weight;
  const int transpose = 0;
                              // obtained results
  int ifound = 0;
  double c_error;
  int * vect_clusterid;

  double ** centroids_data;
  int ** centroids_mask;
  double ** dist_matrix = NULL;
  int * centroids_map;
  int c_mapped, c_row;
  int i, j;

  //// Check parameters and input conversion
  if (!input_parameters(this, 0,
                        rows_data, cols_data, kclusters, npass,
                        method, methods, dist, &vect_c_weight,
                        &mat_c_data, &mat_c_mask))
    return;
  if (method!='d' && dist!='e' && dist!='b')
    Warning("Recomended distances for this method are euclidean ones");
 
  //// Clustering process .....
                                 // vector of cluster assignments
  vect_clusterid = new int[rows_data];
  if (method == 'd') {     // medoids
    if (!(dist_matrix = distancematrix(rows_data, cols_data, mat_c_data,
          mat_c_mask, vect_c_weight, dist, transpose))) {
      Error("Not enought memory for clustering proccess");
      del_input_data(rows_data, mat_c_data, mat_c_mask,
                     vect_c_weight, vect_clusterid);
      return;
    }
    kmedoids(kclusters, rows_data, dist_matrix, npass,
             vect_clusterid, &c_error, &ifound);
  }
  else
    kcluster(kclusters, rows_data, cols_data, mat_c_data, mat_c_mask,
      vect_c_weight, transpose, npass, method, dist,
      vect_clusterid, &c_error, &ifound);
  if ((!ifound) || (ifound==-1)) {
    del_input_data(rows_data, mat_c_data, mat_c_mask,
                   vect_c_weight, vect_clusterid);
    del_dist_matrix(rows_data, dist_matrix);
  }
  if (!ifound) {
    Error("Inconsistents data for clustering proccess");
    return;
  }
  if (ifound==-1) {
    Error("Not enought memory for clustering proccess");
    return;
  }
  c_error /= rows_data;
                                 // centers of classes
  centroids_data = new double*[kclusters];
  centroids_mask = new int*[kclusters];
  for (i=0; i<kclusters; i++) {
    centroids_data[i] = new double[cols_data];
    centroids_mask[i] = new int[cols_data];
  }
  if (method != 'd') {     // means & medians
    if (!getclustercentroids(kclusters, rows_data, cols_data,
                             mat_c_data, mat_c_mask, vect_clusterid,
                             centroids_data, centroids_mask,
                             transpose, method)) {
      Error("Not enought memory for clustering proccess");
      del_input_data(rows_data, mat_c_data, mat_c_mask,
                     vect_c_weight, vect_clusterid);
      del_centroids_data(kclusters, centroids_data, centroids_mask);
      return;
    }
  }
  else {
    centroids_map = new int[rows_data];
    for (i=0; i<rows_data; i++)
      centroids_map[i] = -1;
    c_mapped = 0;
    for (i=0; i<rows_data; i++) {
      c_row = vect_clusterid[i];
      if (centroids_map[c_row] == -1) {
        centroids_map[c_row] = c_mapped;
        for (j=0; j<cols_data; ++j) {
          centroids_mask[c_mapped][j] = mat_c_mask[c_row][j];
          if (mat_c_mask[c_row][j])
            centroids_data[c_mapped][j] = mat_c_data[c_row][j];
        }
        c_mapped++;
      }
      vect_clusterid[i] = centroids_map[c_row];
    }
    delete[] centroids_map;
  }
  //// Create output results
  make_output(contens_, c_error, ifound, kclusters, rows_data, cols_data,
                        centroids_data, centroids_mask, vect_clusterid,
                        mat_c_data, mat_c_mask);
  // free memory    
  del_input_data(rows_data, mat_c_data, mat_c_mask,
                 vect_c_weight, vect_clusterid);
  del_centroids_data(kclusters, centroids_data, centroids_mask);
  del_dist_matrix(rows_data, dist_matrix);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetHierarchicalCluster);
DefExtOpr(1, BSetHierarchicalCluster, "HierarchicalCluster", 2, 5,
  "Matrix Real Text Text Matrix",
  "(Matrix data, Real k [, Text method=\"s\", Text dist=\"e\", Matrix weigth])",
  "Divides the rows of matrix 'data' into 'k' clusters."
  " Unknown values (?) are accepted.\n"
  "Unlike the EM algorithm, which is used in Cluster() function, the complete process of hierarchical clustering is deterministic.\n"
  "Optional parameters are:\n"
  "  method - specifies the method used to calculate the distance between subnodes\n"
  "    \"s\" single (the shortest distance among the pairwise distances between the members of the two nodes),\n"
  "    \"m\" maximum (the longest), \"a\" average, \"c\" centriod\n"
  "  dist - available functions to measure similarity or distance between data\n"
  "    \"e\" Euclidean, \"b\" City-block, \"c\" Pearson correlation coeficient, \"a\" Absolute value of Pearson correlation coeficient,\n"
  "    \"u\" Uncentered Pearson correlation, \"x\" Absolute uncentered Pearson correlation, \"s\" Spearman큦 rank correlation, \"k\" Kendall큦, \"h\" Shape\n"
  "  weight - the weights that are used to calculate the distance\n\n"
  
  "Returns a set whose elements are:\n"
  "  Real AverageDistortion - The average of distances of the items to their centroid\n"
  "  Real TimesSolutionFound - It큦 here just for compatibility with cluster()큦 output, it will always be 1\n"
  "  Matrix Assignments - This array stores the cluster number to which each item was assigned\n"
  "  Matrix Centroids - It stores the centroids of the clusters\n"
  "  Set SetOfClusters - Each subset of this set represents one cluster with its named elements",
          BOperClassify::DataMining_);
//--------------------------------------------------------------------
void BSetHierarchicalCluster::CalcContens()
//--------------------------------------------------------------------
{
  int kclusters;              // given data
  char methods[] = "smac";
    // s - single
    // m - maximum
    // a - average
    // c - centriod
  char method = 's';
  char dist = 'e';
  int rows_data, cols_data;
  double ** mat_c_data;
  int ** mat_c_mask;
  double * vect_c_weight;
  const int transpose = 0;
  int npass;  // just for compatibility with cluster() on input큦 check 

  double ** dist_matrix = NULL;
  Node * tree;
                              // obtained results
  int * vect_clusterid;
  double c_error;
  int ifound = 1;  // just for compatibility with cluster() on output
  double ** centroids_data;
  int ** centroids_mask;
  int i, k;
  
  //// Check parameters and input conversion
  if (!input_parameters(this, 1,
                        rows_data, cols_data, kclusters, npass,
                        method, methods, dist, &vect_c_weight,
                        &mat_c_data, &mat_c_mask))
    return;
  if (method=='c' && dist!='e' && dist!='b')
    Warning("Recomended distances for this method are euclidean ones");
  
  //// Clustering process .....
  tree = treecluster(rows_data, cols_data, mat_c_data, mat_c_mask,
                     vect_c_weight, transpose, dist, method, dist_matrix);
  if (!tree) {
    Error("Not enought memory for clustering proccess");
    del_input_data(rows_data, mat_c_data, mat_c_mask,
                   vect_c_weight, NULL);
    return;
  }
                                 // vector of cluster assignments
  vect_clusterid = new int[rows_data];
  cuttree(rows_data, tree, kclusters, vect_clusterid);
                                      // centers of classes
  centroids_data = new double*[kclusters];
  centroids_mask = new int*[kclusters];
  for (i=0; i<kclusters; i++) {
    centroids_data[i] = new double[cols_data];
    centroids_mask[i] = new int[cols_data];
  }
  getclustercentroids(kclusters, rows_data, cols_data,
                      mat_c_data, mat_c_mask, vect_clusterid,
                      centroids_data, centroids_mask, transpose, 'a');
                                // calculate c_error
  double (*metric)
    (int, double**, double**, int**, int**, const double[], int, int, int) =
       setmetric(dist);
  for (c_error=0.0, k=0; k<kclusters; k++)
    for (i=0; i<rows_data; i++)
      if (vect_clusterid[i] == k)
        c_error += metric(cols_data, mat_c_data, centroids_data,
                      mat_c_mask, centroids_mask, vect_c_weight, i, k, transpose);
  c_error /= rows_data;
  //// Create output results
  make_output(contens_, c_error, ifound, kclusters, rows_data, cols_data,
                        centroids_data, centroids_mask, vect_clusterid,
                        mat_c_data, mat_c_mask);
  // free memory    
  del_input_data(rows_data, mat_c_data, mat_c_mask,
                 vect_c_weight, vect_clusterid);
  del_centroids_data(kclusters, centroids_data, centroids_mask);
}
