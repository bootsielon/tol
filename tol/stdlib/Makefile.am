## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2008, Bayes Forecast, SA
## Written by Daniel Rus Morales <daniel@uponeits.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation, 
## Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

pkgconfigdir	= $(libdir)/tol1.1.7/stdlib
pkgconfig_DATA	= \
_init_stdlib.tol _packages.tol

datadir		= $(libdir)/tol1.1.7/stdlib/data
data_DATA	= \
data/_init_data.tol

oisdir		= $(libdir)/tol1.1.7/stdlib/data/ois
ois_DATA	= \
data/ois/_init_ois.tol	data/ois/_ois.tol

generaldir	= $(libdir)/tol1.1.7/stdlib/general
general_DATA	= \
general/_constant.tol  general/_init_general.tol

grammarsdir	= $(libdir)/tol1.1.7/stdlib/general/grammars
grammars_DATA	= \
general/grammars/_anything.tol       general/grammars/_real.tol \
general/grammars/_date.tol           general/grammars/_serie.tol \
general/grammars/_init_grammars.tol  general/grammars/_set.tol \
general/grammars/_matrix.tol         general/grammars/_special.tol \
general/grammars/_polyn.tol          general/grammars/_text.tol \
general/grammars/_ratio.tol          general/grammars/_timeset.tol

systemdir	= $(libdir)/tol1.1.7/stdlib/general/system
system_DATA	= \
general/system/_deprecated.tol   general/system/_system_sample.tol \
general/system/_init_system.tol  general/system/_system.tol

tpadir		= $(libdir)/tol1.1.7/stdlib/general/system/tpa
tpa_DATA	= \
general/system/tpa/_cycle.tol	  general/system/tpa/_.hostList.bst.tpl \
general/system/tpa/_init_tpa.tol  general/system/tpa/_server.tol

recursive_dirdir = $(libdir)/tol1.1.7/stdlib/general/system/recursive_dir
recursive_dir_DATA 	= \
general/system/recursive_dir/_example_find.tol \
general/system/recursive_dir/_functions.tol \
general/system/recursive_dir/_init_recursive_dir.tol \
general/system/recursive_dir/_structs.tol

tol_testsdir = $(libdir)/tol1.1.7/stdlib/general/tol_tests
tol_tests_DATA = \
general/tol_tests/_init_tol_tests.tol  general/tol_tests/_testFunc.tol \
general/tol_tests/_testConfig.tol      general/tol_tests/_testStruct.tol \
general/tol_tests/_toltclTester.tcl

variousdir = $(libdir)/tol1.1.7/stdlib/general/various
various_DATA 	= \
general/various/_axistimeset.tol   general/various/_plotter.tol \
general/various/_init_various.tol  general/various/_sortselect.tol \
general/various/_tools.tol         general/various/regexp.tol

guidir = $(libdir)/tol1.1.7/stdlib/gui
gui_DATA 	= \
gui/_init_gui.tol

htmldir = $(libdir)/tol1.1.7/stdlib/gui/html
html_DATA 	= \
gui/html/_htmlmethods.tol  gui/html/_htmlutil.tol  gui/html/_init_html.tol

tcldir = $(libdir)/tol1.1.7/stdlib/gui/tcl
tcl_DATA 	= \
gui/tcl/_buttonbartest.tol  gui/tcl/_messageboxtest.tol \
gui/tcl/_buttonbar.tol      gui/tcl/_messagebox.tol \
gui/tcl/_charttest.tol      gui/tcl/_progressbartest.tol \
gui/tcl/_chart.tol          gui/tcl/_progressbar.tol \
gui/tcl/_entrytest.tol      gui/tcl/_tabulatetest.tol \
gui/tcl/_entry.tol          gui/tcl/_tabulate.tol \
gui/tcl/_guifunction.tol    gui/tcl/_tclutils.tol \
gui/tcl/_init_tcl.tol

xlsdir = $(libdir)/tol1.1.7/stdlib/gui/xls
xls_DATA	= \
gui/xls/_init_xls.tol

mathdir = $(libdir)/tol1.1.7/stdlib/math
math_DATA	= \
math/_init_math.tol

Rdir	= $(libdir)/tol1.1.7/stdlib/math/R
R_DATA	= \
math/R/_init_Rapi.tol	math/R/Rapi.tol

optimdir = $(libdir)/tol1.1.7/stdlib/math/optim
optim_DATA	= \
math/optim/_factpolyn.tol  math/optim/_init_optim.tol  math/optim/_marquardt.tol

LPdir	= $(libdir)/tol1.1.7/stdlib/math/optim/LP
LP_DATA	= \
math/optim/LP/_init_LP.tol	math/optim/LP/Rglpk.tol

QPdir	= $(libdir)/tol1.1.7/stdlib/math/optim/QP
QP_DATA	= \
math/optim/QP/_init_QP.tol	math/optim/QP/Rquadprog.tol

statdir = $(libdir)/tol1.1.7/stdlib/math/stat
stat_DATA	= \
math/stat/_init_stat.tol

kdedir = $(libdir)/tol1.1.7/stdlib/math/stat/kde
kde_DATA	= \
math/stat/kde/_init_kde.tol	math/stat/kde/_rkde.tol \
math/stat/kde/test.tol		math/stat/kde/tolRkde.R

descriptive_statsdir = $(libdir)/tol1.1.7/stdlib/math/stat/descriptive_stats
descriptive_stats_DATA	= \
math/stat/descriptive_stats/_init_descriptive_stats.tol

hypothesis_testsdir = $(libdir)/tol1.1.7/stdlib/math/stat/hypothesis_tests
hypothesis_tests_DATA	= \
math/stat/hypothesis_tests/_confidence_intervals.tol \
math/stat/hypothesis_tests/_init_hypothesis_tests.tol

modelsdir = $(libdir)/tol1.1.7/stdlib/math/stat/models
models_DATA	= \
math/stat/models/_init_models.tol

bayesiandir = $(libdir)/tol1.1.7/stdlib/math/stat/models/bayesian
bayesian_DATA	= \
math/stat/models/bayesian/_init_bayesian.tol

arimadir = $(libdir)/tol1.1.7/stdlib/math/stat/models/bayesian/arima
arima_DATA	= \
math/stat/models/bayesian/arima/_bmtsarima_sample.tol \
math/stat/models/bayesian/arima/_bmtsarima.tol \
math/stat/models/bayesian/arima/_init_arima.tol

diagnosisdir	= $(libdir)/tol1.1.7/stdlib/math/stat/models/bayesian/diagnosis
diagnosis_DATA	= \
math/stat/models/bayesian/diagnosis/_init_diagnosis.tol \
math/stat/models/bayesian/diagnosis/_rcoda.tol

glindir = $(libdir)/tol1.1.7/stdlib/math/stat/models/bayesian/generalized_linear
glin_DATA	= \
math/stat/models/bayesian/generalized_linear/_bprobit.tol \
math/stat/models/bayesian/generalized_linear/_init_generalized_linear.tol \
math/stat/models/bayesian/generalized_linear/_probit_sample.tol

lineardir = $(libdir)/tol1.1.7/stdlib/math/stat/models/bayesian/linear
linear_DATA	= \
math/stat/models/bayesian/linear/_blr_add.tol \
math/stat/models/bayesian/linear/_blr_functions.tol \
math/stat/models/bayesian/linear/_blr_kernel.tol \
math/stat/models/bayesian/linear/_blr_sample.tol \
math/stat/models/bayesian/linear/_blr_struct.tol \
math/stat/models/bayesian/linear/_blrc_kernel.tol \
math/stat/models/bayesian/linear/_init_linear.tol

mledir = $(libdir)/tol1.1.7/stdlib/math/stat/models/mle
mle_DATA	= \
math/stat/models/mle/_init_mle.tol

mlearimadir = $(libdir)/tol1.1.7/stdlib/math/stat/models/mle/arima
mlearima_DATA	= \
math/stat/models/mle/arima/_constants.tol \
math/stat/models/mle/arima/_fun_combination.tol \
math/stat/models/mle/arima/_fun_decision.tol \
math/stat/models/mle/arima/_fun_forecasting.tol \
math/stat/models/mle/arima/_fun_general.tol \
math/stat/models/mle/arima/_fun_isolate.tol \
math/stat/models/mle/arima/_fun_simulate.tol \
math/stat/models/mle/arima/_fun_transformation.tol \
math/stat/models/mle/arima/_init_arima.tol \
math/stat/models/mle/arima/_structs.tol \
math/stat/models/mle/arima/_diagbounds.bst

aiadir = $(libdir)/tol1.1.7/stdlib/math/stat/models/mle/arima/aia
aia_DATA	= \
math/stat/models/mle/arima/aia/_build_model.tol  \
math/stat/models/mle/arima/aia/_init_aia.tol \
math/stat/models/mle/arima/aia/_constants.tol \
math/stat/models/mle/arima/aia/_step_searcher.tol \
math/stat/models/mle/arima/aia/_functions.tol \
math/stat/models/mle/arima/aia/_structs.tol
