# pyTol: feasibility assessment and option set

> Companion to `SPEC_SHEET.md`. Written 2026-09-06 from a full review of the `bootsielon/tol` repository at HEAD `3026db38`, three targeted explorations (package ecosystem, embedding API, capability inventory) and three independent option designs. Claims cite the file and line they come from; effort figures are estimates for one experienced engineer and should be read as relative sizes.

## 1. Summary

**The question.** Can TOL's best ideas be carried into Python as "pyTol", and if so how: a clean-room Python library, a binding to the existing C++ kernel, a TOL-like language hosted on Python, or all three?

**The short answer.** Build a clean-room Python library around TOL's genuinely distinctive core, use the old kernel only as a temporary test oracle, and treat the language layer as tooling rather than as a product. The distinctive core is small and well specified by this repository; roughly two thirds of the kernel is commodity that numpy, pandas, scipy and statsmodels already provide, or dead code. The proprietary modelling layer that made TOL a product (MMS, BysMcmc, the StdLib calendars) is not in this repository and has to be re-derived from references, docs and the user's own memory.

**The ranked option set** (details in sections 5-8):

| Rank | Option | Effort | Risk | Payoff | Verdict |
|---|---|---|---|---|---|
| 1 | **D-phased**: A as the core; B only as a two-week build spike that becomes a differential oracle if it passes; C only as tooling inside A (census, parser, Pygments; DSL deferred) | ~50-60 person-weeks to a 1.0; the time-set/series layer is usable by week 9-12 | moderate | highest | **Recommended** |
| 2 | **A**: clean-room Python library | ~44 person-weeks (plan 50-55) | moderate: ARIMA numerical parity, AIA fidelity, BSR sampler without an oracle | high | The destination |
| 3 | **B**: bind the C++ kernel | ~8 person-weeks for a Linux developer tool, ~14-16 for distributable packages; GPL-3.0 | high: 1998-era toolchain, 64-bit, one interpreter per process, ~10 native dependencies | medium: runs kernel-only legacy code today, but most 2013-2015 user code still needs the absent packages | A bridge, not a destination; wins only if old packages are recovered and running them is the main goal |
| 4 | **C**: TOL-like language on Python | ~6 person-weeks for the tooling subset, ~18 for the full option | high: semantic drift in transpiled code, and nothing legacy to run | low as a standalone | Not viable alone; the embedded Python API already reads as well as a new syntax |
| — | **D-naive**: build all three fully | ~80-90 person-weeks | highest | same as D-phased | Rejected: roughly double the cost for the same value |

**Priority alignment.** The user ranked (1) time-set algebra and calendar-aware series, (2) forecasting and econometrics, (3) MMS-style model management, (4) Bayesian hierarchical models. Option A's roadmap delivers them in that order; the first two need no material outside this repository, the third is reconstructable from references, the fourth has its design documented in 17 LyX chapters but its estimator is absent.

## 2. What this repository is for pyTol

The kernel should not be carried forward as code. It is strict C++98/03 with hard errors on a modern toolchain (141 uses of `register`, dynamic exception specifications in a core header `tol/btol/timeset_type/tol_btmsgra.h:550-773`), every piece of interpreter state is a class static (`tol/btol/bgrammar/gra.cpp:37-51`, `tol/bbasic/tol_bout.h:64-118`), nothing is locked, `LoadInitLibrary` can run once per process (`tol/init.cpp:1306-1308`), initialisation installs process-wide signal handlers that call `exit()` on Windows (`tol/init.cpp:293-334`), and the link line needs GSL, CHOLMOD, FFTW, Boost, ZipArchive, bzip2, zlib, libltdl and BLAS/LAPACK (`tol/CMakeLists.txt:442-447`). It is licensed GPLv3 (`tol/COPYING`), with GPLv2 headers and several bundled libraries under other terms (`SPEC_SHEET.md` 1.2, 10.5).

The repository is nevertheless an unusually good *specification*, because the things worth keeping are precisely the ones the code states exactly:

| Asset | Where | What it specifies |
|---|---|---|
| The TimeSet contract | `tol/btol/timeset_type/tol_btmset.h` (`Contain`, `Successor/Predecessor`, `FirstNoLess/FirstNoGreat`, `Difference`, `GetHashBetween`, the cache API), node classes in `tol_btmsgrav.h`, constructors and their help text in `tmsgrav.cpp` | a lazy, possibly infinite calendar set with successor arithmetic and a windowed cache |
| Series semantics | `tol/btol/serie_type/tsrgrav.cpp` (`DatCh` :439-531, `SubSer` :660-780, `Polyn:Serie` :782-892, `DifEq` :896-1070, regressors :177-438), alignment in `srg.cpp:108-177` | per-date lazy evaluation, intersection alignment, resampling with arbitrary aggregators, lag-operator application, rational filters with initial values |
| Lag-operator algebra | `tol/bmath/mathobjects/tol_bpolyn.h`, `tol/btol/polynomial_type/polgra.cpp`, `ratio_type/`, `polmat_type/`, `tol/bmath/bstat/ar.cpp`, `unitroot.cpp` | Laurent polynomials in B and F, Schur stationarity, unit-root identification over candidate difference operators, polynomial matrices |
| ARIMA with transfer functions | `tol/btol/bmodel/` (`tol_bmodel.h`, `modcalc.cpp`, `estim.cpp`), `tol/bmath/bstat/arma.cpp`, `tol/stdlib/arima/_structs.tol`, `_diagbounds.bst` | the model structure, exact Levinson likelihood, per-parameter Gaussian priors, missing-value integration, four likelihood evaluators, diagnostic accept/refuse bands |
| Automatic intervention analysis | `tol/btol/bmodel/aia.cpp` (:459-534 greedy loop, :717-744 dispatch, :540 registration), `doc/modeling/AIA/` | an algorithm documented nowhere else in the Python ecosystem |
| The BSR model language | `tol/btol/vmatrix_type/tol_bvmat_bsr_err.h:119-176` (syntax templates), `tol_bvmat_bsr.h`, `doc/modeling/BSR/BSR_P2_S3_Formato_ASCII_BSR.lyx`, fixtures under `tol_tests/tol/BSR/test_{11,13,14,18,21}` | sparse design, linear inequality constraints, ARMA-structured noise, missing values as parameters, modular models |
| Constrained sampling | `vmatrix_type/vmatgra.cpp:2690-2731` (`TruncStdGaussian`, `GetBoundsInPolytope`), `vmat_chol.cpp` (Cholesky with row add/delete), `tol/bmath/gsl_ext/` | polytope-constrained Gaussian draws and sparse Cholesky updates |
| The grammar | `tol/bparser/scn.cpp:260-390` (token tables: 5 bracket pairs, 2 separators, 32 binary tokens in 18 precedence levels, 10 monary, 19 type keywords) | enough to generate a lexer or a parser mechanically |
| Tests | `tol_tests/tol`: 480 `test.tol` drivers, 135 data fixtures, `ARIMA/SimulArima` (simulate with known parameters, then estimate), 30 BSR directories, 348 Bugzilla regressions | property tests and golden inputs; note that tests compute `quality_` at run time and store no expected numbers |
| Design documents | `doc/modeling/BSR/*.lyx` (17 chapters), `doc/modeling/ARIMA_Estimate/`, `doc/kernel/Time/TD-diseño.lyx`, the bilingual `I2(en, es)` help string on every built-in | the normative text for the Bayesian layer, the estimator, the time algebra, and the reference manual |

What should be dropped without regret: the never-built trees (`tol/tai`, `bkfilter`, `ctime_type`, `ctimeset_type`, `cseries_type`, `csparse`, about 12,400 lines), the 9,287-line generated GSL wrapper layer hard-coded to GSL 1.8, the 28 probability distributions on one global random generator, the home-grown optimisers with known logic errors (`Bisection`, `MullerMethod`, `NewtonSolve`), the vendored SQLite amalgamation and database layer, the flat-file `@BDB` database, the Excel BIFF reader (unbuilt), CINT, Triangle, ANN with pointers punned into doubles, the Tcl-based regex, the `Tpa` remote-execution farm (an unauthenticated daemon), and OIS itself (Python gets `pickle` and `.pyc` for free; keep a read-only `.oza` reader for migration only).

## 3. MMS and the package ecosystem

`SPEC_SHEET.md` 3.8 has the full evidence; the essentials:

- **MMS is not in this repository and never was.** It was Bayes Forecast's private product: 47 versions are catalogued in `tol/stdlib/TolPackage/listA.oza`, its version control was `https://trac.localbayes.es/svn/mms/`, its distribution point the private repository `http://packages.localbayes.es/BayesPrivateTolPackages/repository.php`, its dependencies `BysMcmc, QltvRespModel, RandVar, DecoTools`. Fifteen of the 69 catalogued packages were private, and no package source of any kind is in the tree.
- **Its object model is reconstructable.** From `tol_tests/tol/Bugzilla/bug_001608/runMms.tol` and `TolJava/TOLJava-Ejemplos/.../Ejemplo10.java`: a `Container` registry; `@DataSet` whose variables are expressions with tags; `@Model`/`@Submodel` with Box-Cox transformation labels, explanatory terms carrying transfer-function polynomials and an additive/multiplicative flag, and per-parameter priors; `@Estimation` with interchangeable `@SettingsMultiMLE` and `@SettingsBSR` engines behind one `Execute`; `SaveIsolationFile` for reproducibility; a forecast side that walks submodels, observations and explanatory-term decompositions with scenario variables named by suffix. This is enough to design the model-management layer of pyTol (section 5.4).
- **Its requirements ancestry survives** in `doc/general/mole-spec/herramientas_de_modelacion.odt` (2006), which names the pains MMS was built to solve: a model-specification interface, storing estimated parameter chains, reference diagnostics, parallel hardware, and Web/VB/Excel/Tcl/Java interfaces.
- **BysMcmc is also absent** and every BSR test requires it, so the Bayesian estimator has to be rebuilt from the 17 LyX chapters. The named calendars (`Weekly`, `Monthly`, `Quarterly`, `Yearly`, …) came from the `StdLib` package and are absent too; the kernel defines only `Daily`, `Easter` and the empty set (`tol/btol/timeset_type/tmsgra.cpp:171-175`).

**What to look for in old installations.** The package client kept installed packages under `<TolAppDataPath>/TolPackage.4/Client/<package>/` with `<package>.oza` and `info.oza` inside (`tol/stdlib/TolPackage/client.tol`; on Windows the app-data path is under `%APPDATA%`, on Unix under `$HOME`). Directories named `MMS.*`, `BysMcmc.*`, `StdLib.*`, `RandVar.*`, `DecoTools.*`, `QltvRespModel.*` are the prize; `.oza` model containers and `.bsr` files from real projects are the second prize; any `_iniproject.tol` or `.tolConfig.*.tol` files show how projects were wired. OIS images are readable by the kernel binding (section 6) and, through it, convertible to parquet.

## 4. Corrections to the working recollection

- **vbtol** is a Win32 DLL with 18 `__stdcall` exports reached from Excel/VB6/VBA through plain `Declare` statements (`vbtol/vbasic/tol.bas`, `vbtol/source/vbtol.def`). It is not a COM server, it is 32-bit only, and its installer template packages a client workbook with the TOL 1.1.7 runtime (`SPEC_SHEET.md` 5.6).
- **rtol** builds one shared library, `tolRlink`, with 13 C entry points in R's `.C()` convention (`rtol/source/tolRlink.cpp`). There is no R package: no `DESCRIPTION`, `NAMESPACE` or `.R` file exists in the repository (5.5).
- **TolJava** is an in-process JNI binding plus a JSF 1.2 demo web application that creates one interpreter per HTTP session (5.4). It has no threads, queues or job scheduling, and the tree contains no reference to Hadoop, MapReduce or Spark. It was not a big-data or cloud tool; it was the "Java interface" line item of the 2006 debate paper.
- **TOL's only parallelism** is the stdlib `Tpa` API: `rmtpsd` daemons spawning `tolsh -server` workers, bulk-synchronous round-robin dispatch, data through a shared filesystem (3.5, 3.8). The daemon itself lives in a separate `rmtps` project that is not here. The "clusters (VPM, MPI)" line in the Mole specification was never built.

## 5. Option A: "pytol", a clean-room Python library

### 5.1 Core data model

- **Dates**: `numpy.datetime64[ns]` (int64), timezone-naive "social time" as `doc/kernel/Time/TD-diseño.lyx` frames it, `NaT` for Unknown, two sentinels for TOL's `Begin`/`End`. TOL's `BDate` rounds seconds to 1/100 and hashes a date into one double as `±YYYYMMDD + fraction of day` (`tol/bbasic/dtealgeb.cpp:1030-1049`); int64 nanoseconds are total-ordered, vectorisable and what pandas, polars and Arrow already speak.
- **TimeSet**: a lazy, possibly infinite date set implementing exactly the `BTimeSet` contract: `contains`, `succ(d, n)`, `pred(d, n)`, `first_no_less`, `first_no_greater`, `difference`, `between(d1, d2) -> int64[]`. Node classes mirror `tol_btmsgrav.h` one to one (`Daily`, `Easter`, `WeekDay`, `MonthDay`, `Month`, `Hour`, `Minute`, `Second`, `Day`, `Year`, `In`, `Union`, `Intersection`, `Difference`, `Periodic`, `Succ`, `Range`, `DatesOf`, `OfSerie`), with Python `| & -` and TOL's `+ * -` as aliases. Two implementation tiers: a closed-form vectorised `between()` for regular sets (anything reducible to a fixed grid on `Daily`), and a generic tier that iterates `succ()` behind the same windowed LRU cache TOL uses (`tol/btol/timeset_type/tms.cpp:161-502`, with a global byte budget). Unbounded scans become a bounded-probe `EmptyTimeSetError`. The named calendars are authored fresh as compositions (Weekly = `WD(1)`, Monthly = `D(1)`, Quarterly = `D(1) & (M(1)|M(4)|M(7)|M(10))`, Fortnightly = `D(1)|D(16)`, Decenial = `D(1)|D(11)|D(21)`, …) from the list in `tol/stdlib/various/_axistimeset.tol:14-25`; **their exact definitions must be confirmed with the user**. `TestCoherenceOfTimeSet` becomes a `hypothesis` property.
- **Serie**: a lazy expression graph with TOL's per-date evaluation semantics; `values(first, last)` materialises a window. Alignment is the **intersection** of ranges on a common dating, exactly as `BSerieGroup::PrepareOperate` (`srg.cpp:108-177`). TOL's dating-compatibility check walks dates one by one (`tms.cpp:520-544`); pytol replaces it with structural equality of hash-consed TimeSet expressions plus an optional strict walk. Preserved verbatim: `subser` (the explicit materialisation point), `datch(target, stat)` (value at `d` is `stat` over `[d, pred_source(succ_target(d))]`, vectorised with `np.add.reduceat`, any callable accepted, harmonic check enforced), `LagPoly @ serie` (TOL's `:`), `difeq(ratio, s, init)` via `scipy.signal.lfilter` with initial state, `concat`/`<<`/`>>`, and the regressor vocabulary `CalInd`, `CalVar`, `Pulse`, `Step`, `Compens`, `Trend`, `Line`. **Unknown is NaN in float64**, as in TOL itself (`tol/bmath/mathobjects/tol_bdat.h:49-57`), with three-valued logic in a `pytol.logic` module; `to_pandas(nullable=True)` yields `pd.NA`.
- **LagPoly, Ratio, PolMatrix**: sparse `{degree: coefficient}` with negative degrees so `B` and `F` coexist (`tol_bpolyn.h:596-617`); `change_bf`, `inverse_roots`, `period()` (GCD of degrees for seasonal detection), roots after period reduction (as `complex.cpp:427-453`), Schur-recursion stationarity (`bmath/bstat/ar.cpp`), `extract_period`, `random_stationary`, `inverse_nonstationary_roots`; `Ratio.expand(n)` for ψ-weights, `:` composition, `adjust_quotient`; `PolMatrix` as a `float64[lag, i, j]` array with about 15 methods covering the 35 builtins in `polmatgra.cpp`.

Intended API, to be read as a target rather than a promise:

```python
import pytol as pt
from pytol import B, Daily, Monthly, WD, M, D, Easter

biz = Daily - (WD(6) | WD(7)) - Easter.succ(-2, Daily)      # business days minus Good Friday
sales = pt.Serie.from_pandas(df["sales"], dating=Daily)
y = pt.log(sales.datch(Monthly, "sum")).subser("2015-01-01", "2024-12-01")
dy = (1 - B) * (1 - B**12) @ y                                # TOL: ((1-B)*(1-B^12)):y
holy = pt.CalInd(Easter.range(-7, 0, Daily), Daily).datch(Monthly, "sum")
spec = pt.arima.Spec(
    output=y, boxcox=(0, 0), period=12, dif=(1 - B) * (1 - B**12),
    ar=[1 - 0.5*B], ma=[1 - 0.5*B, 1 - 0.5*B**12],
    inputs=[pt.arima.Transfer(omega=pt.Ratio(0.1, 1 - 0.7*B), x=holy)],
    priors={"MA1.1": pt.Normal(0.5, 0.1)},
)
fit = pt.arima.estimate(spec, first="2015-01-01", last="2024-12-01")
shapes = pt.aia.detect(fit.residuals, fit.ratio, max_order=2, max_cross=100)
fc = pt.arima.estimate(spec.with_inputs(shapes)).forecast(until="2025-12-01").to_pandas()
```

### 5.2 Econometrics

Specs are typed dataclasses mirroring `@ModelDef`, `@InputDef` and `@NonLinearInputDef` (`tol/stdlib/arima/_structs.tol:7-82`), plus a re-authored `ArimaFactor` standing in for the external `@ARIMAStruct`.

| Capability | TOL source of truth | pytol decision |
|---|---|---|
| Exact likelihood | `tol/bmath/bstat/arma.cpp`; `CalcLikelihood` always selects Levinson | port Levinson to numba (~250 lines); Almagro skipped (never selected), Whittle and Gohberg-Semencul optional |
| Optimiser | `LeastSqrMarquardt` on the Schur-reparametrised vector (`estim.cpp:2380`) | `scipy.optimize.least_squares(method="lm")`; the PACF bijection equals `statsmodels...constrain_stationary_univariate` |
| Priors and restrictions | appended residual rows (`tol_bmodel.h:126-130`) | append `(θ-ν)/σ` rows so LM sees a MAP problem, TOL's own trick |
| Transfer inputs | `Omega(B):X`, rational ω via `Ratio` | `Transfer(omega: LagPoly|Ratio, x)`; rational ω applied by `difeq` inside the residual function (statsmodels cannot express denominators) |
| Non-linear inputs | user `Code` returning a Serie | `NonLinear(params, fn)` joining the LM vector |
| Seasonal factors, Box-Cox | any number of factors; `(λ1, λ2)` | same |
| Missing values | `IntegrateMissingValues` (`modcalc.cpp:1815`); tests use pulse inputs (`ARIMA/Missing/MissingEstimation.tol:59-67`) | v1 pulse inputs; v2 missing values as LM parameters |
| Diagnostics | `moddiag.cpp`, 11 bands in `_diagbounds.bst` | bands ported verbatim as defaults; Ljung-Box from statsmodels, `BoxPierceMod` own |
| Forecast | `TransPrev` (`modprev.cpp:199`) | ψ-weight forecasting with input futures, cross-checked against `SARIMAXResults.get_forecast` |
| AIA | `aia.cpp` greedy loop and beam search | **new code**: `aia.detect(res, ratio, shapes={pulse: 1/1, compens: (1-B)/1, step: 1/(1-B)}, max_order, max_cross, min_nonzero_prob)`; nearest existing analogue is R's `tsoutliers` |
| Unit-root identification | `unitroot.cpp:142-165` (3 or 9 candidate difference operators, Monte-Carlo with t-draws) | new, ~150 lines |
| GARCH | none in TOL | adapter to the `arch` package on residuals |

`estimate(spec, engine="pytol" | "statsmodels")`: the statsmodels engine works whenever the spec is expressible there (one seasonal factor, polynomial ω, no priors) and stays in CI as a permanent parity oracle. pmdarima and statsforecast are not engines; their order search is exposed as `arima.auto_spec()`.

### 5.3 Bayesian layer

A `lark` grammar written from the syntax templates in `tol_bvmat_bsr_err.h:119-176` and the format chapter `BSR_P2_S3_Formato_ASCII_BSR.lyx` (accepting both the `ARMA.COV` spelling in the help text and the `ARIMA.COV` spelling in the fixtures), producing a `BsrModelDef` that mirrors `@BSR.ModelDef` with the design and constraint matrices as `scipy.sparse.csc_matrix`, and a Python builder producing the same object. The sampler is an own block Gibbs following chapters `BSR_P1_S2..S6`, because the combination of sparse design, polytope constraints, ARMA-structured covariance and missing values as parameters has no natural PyMC or NumPyro rendering and HMC on a polytope is the wrong tool: linear block via `scikit-sparse` (CHOLMOD, the library TOL uses) and a numba coordinate-Gibbs on the truncated multinormal, variance block inverse-χ², ARIMA block with a slice sampler, missing block as a joint conditional normal, non-linear block Metropolis-within-Gibbs. Chains go to `arviz.InferenceData`; configuration keys keep the names in `BSR_P2_S1` so settings such as those in `runMms.tol:150-154` map directly. The LyX chapters are converted once into `docs/bsr/` as the normative specification.

### 5.4 Model management (MMS-style)

Reconstructed from `runMms.tol` and `Ejemplo10.java`: `DataSet` with expression-valued, tagged `Variable`s; `Model`/`Submodel` with transformation labels (`BoxCox_0_0`), noise labels parsed both ways (`P<periods>DIF<orders>AR<orders>MA<orders>`), `TransferTerm(input, transfer, additive)` and `parameter(name).set_prior(mean, sigma)`; `Estimation(model, MLESettings | BSRSettings).execute()` returning a common `EstimationResult`; `forecast(until, scenarios)` with per-submodel observations and explanatory-term decompositions; a `Container` (duckdb or sqlite catalogue, parquet for series, pydantic JSON for specs, arviz storage for chains) with `replace_*`, `get_model` and `load`; `bundle()` as the `SaveIsolationFile` analogue (specs, inputs, versions, lockfile, seeds). Adapters implement sktime's `BaseForecaster` and a darts wrapper; mlflow and dvc are optional.

### 5.5 Layout, dependencies, tests, roadmap

`pytol/{dates, timeset, serie, lagpoly, arima, aia, bsr, mms, io/legacy, adapters, logic}`, about 10-12k lines of Python replacing about 40k of C++. Core dependencies: numpy, pandas, scipy. Optional extras: `stats` (statsmodels, arch), `fast` (numba), `io` (pyarrow, polars), `bayes` (scikit-sparse, arviz, lark), `mms` (duckdb, pydantic), `ml` (sktime, darts), `track` (mlflow). Tests: hypothesis strategies over random TimeSet expressions checking the algebraic laws, goldens on the 135 fixtures, parity against statsmodels and arch, `SimulArima` recovery tests, simulation-based calibration for BSR. Numeric goldens should be recorded once from a one-off Docker build of `tolsh`, not from a permanent binding.

| Phase | Weeks | Deliverable |
|---|---|---|
| 0 Dates, TimeSet, calendars, Easter, cache, property suite | 4 | `pytol.timeset` 1.0 |
| 1 Serie graph, alignment, `datch`/`subser`, regressors, pandas/polars/arrow adapters, legacy `.bdt/.bst/.bmt/.bdc` readers | 5 | priority 1 complete |
| 2 LagPoly, Ratio, DifEq, PolMatrix, Schur, unit roots | 3 | `pytol.lagpoly` |
| 3 ARIMA: Levinson, LM with Schur map, priors, transfer and non-linear inputs, missing values, diagnostics, forecast, statsmodels engine | 8 | `pytol.arima` with parity report |
| 4 AIA, GARCH adapter, sktime/darts adapters, `auto_spec` | 4 | priority 2 complete |
| 5 MMS layer | 6 | priority 3 |
| 6 BSR parser, builder, Gibbs blocks, arviz reports | 10 | priority 4 |
| 7 Docs, plotting, hierarchical reconciliation, packaging | 4 | 1.0 |

About 44 person-weeks; plan 50-55 with contingency. Phases 0-2 alone already deliver the user's first priority.

Risks, ranked: ARIMA numerical parity (mitigated by statsmodels parity tests, `SimulArima` recovery tests, a one-off golden run); AIA fidelity (spec is `AIA.doc` plus the help text; record goldens on synthetic series with injected outliers); the BSR sampler has no oracle (simulation-based calibration, PyMC on unconstrained sub-cases); irregular TimeSet performance (regular fast tier, cache budget, numba); reconstructed calendars (confirm with the user); MMS scope creep (freeze to what `runMms.tol` shows); scikit-sparse wheels on Windows (dense or `splu` fallback).

## 6. Option B: "pyTol-bind", binding the C++ kernel

### 6.1 Technology

Not ctypes over `tol/tol_init.h`: it exports only lifecycle and version functions; everything a binding needs is C++ with mangled names. Not pybind11 or nanobind over the C++ object model: any translation unit including TOL headers inherits the ABI-affecting define set (`tol/CMakeLists.txt:233-240`; `__USE_POOL__=__POOL_BFSMEM__` changes class layout), macro pollution (`BInt`, `BBool`, `BReal`), and the `register`/throw-specification blockers that pin the extension to `-std=gnu++14` forever. Recommended: a ~60-function `extern "C"` shim (`tol/tolc/tolc.{h,cpp}`) compiled as one more TOL sublibrary so it inherits the right flags, linked with a static libtol into `libtolc` with hidden visibility, and consumed from Python through cffi in out-of-line API mode against the plain C header (typed signatures, zero-copy buffers, GIL released around calls, callbacks via `@ffi.def_extern`, one wheel per platform via the limited API).

The shim owns the process-global hazards: save and restore signal handlers around `InitTolKernel` and route Ctrl-C to `StopFlagOn()` (`tol/lang/tol_blanguag.h:85-87`); leave `LC_NUMERIC=C`; a module-level lock, an idempotent `init`, and a refusal to fork after initialisation; bytes at the boundary with configurable Latin-1 decoding (TOL identifiers are Latin-1 bytes, `tol/bparser/fil.cpp:83-115`); every call returns the error and warning counter deltas (`tol/bbasic/tol_bout.h:36-39`), since errors never throw; `setErrorTags`/`setWarningTags` set sentinel prefixes so the output callback classifies messages without regular expressions.

### 6.2 API and what nobody built before

`tol.init()`, `tol.run(text)`, `tol.eval(text, grammar)`, `tol[name]` and `tol[name] = value` (constructing a named object is the only "set variable" mechanism the kernel has: `tol/btol/bgrammar/tol_bgencon.h:58-63`), `TolObject` handles that `IncNRefs()` on acquisition and `DecNRefs(); Destroy()` on deallocation (a fresh result has zero references and dies on the first `Destroy()`, `tol/bbasic/atm.cpp:82-93`; no existing binding handles this), `tol.call(code, *args)` via `BCode::Evaluator` with no text round trip, `tol.capture()` for output, Python callbacks installed through `InstallTclEval` (a type-agnostic hook, so TOL's `Tcl_Eval` built-in can run Python) and the chart hooks, and offline package loading through `BPackage::Load`.

Three things the four old bindings never did are possible: a **zero-copy numeric bridge** (`BDat` is one `double` with no vtable, `BMatrix<BDat>` is dense row-major contiguous, `BTimeSerie::GetDataBuffer()` yields a `double*`, `BVMat` exposes CHOLMOD sparse arrays, and `BTimeSet::GetHashBetween` enumerates dates in bulk), **parse-free calls** (`tol/TestUseTol.cpp:51-120` shows the pattern), and a **validated object handle** that already exists in the kernel (`GetAddressFromObject`/`GetObjectFromAddress`, `tol/btol/bgrammar/syn.cpp:478-534`).

### 6.3 Build, distribution, scope

Build bbasic, bmath, bparser, btol, lang, LoadDynLib, PackArchive, OIS and the contribs kmlocal, clusterlib, alglib, optimal_bw and ZipArchive; leave out cint, ANN, tpp, excel and dbdrivers. Flags `-std=gnu++14` (modern Boost needs C++11; `register` and `throw()` are only deprecated in 14) with a small patch overlay rather than a fork (honour `BUILD_SHARED_LIBS` at `tol/CMakeLists.txt:438`, make the `prj_zipa` dependency at `:433` conditional, `-DWGET_ERROR_MESSAGE=WARNING` to remove the configure-time download, `-DCLAPACK_NAME=lapacke`, define `UNIX` on Darwin). Linux x86_64 is high confidence; Windows x64 goes through MinGW-w64 and MSYS2; macOS has no support today and is a one-to-two-week add after Linux. Distribute through conda-forge first (every dependency exists there), manylinux wheels with auditwheel second (30-40 MB). Ship `tol/stdlib/**` as package data, point `TOL_LIBRARY` at it (`tol/init.cpp:360`), and always start with default packages disabled, otherwise `DefaultPackages.tol` triggers a remote install against a dead host. **The binding is GPL-3.0.**

Runs today without any external package: every kernel built-in, the in-tree stdlib, `.oza` reading, and most of the test batteries (the runner must be rewritten in pytest because the harness functions are absent). Needs external packages: all BSR tests and `ARIMA/mcmc` (BysMcmc), the MMS references, and anything using the named calendars, which is practically all 2013-2015 user code; a compatibility file with provisional calendar definitions and a provisional `@ARIMAStruct` narrows that gap.

### 6.4 Roadmap, risks, verdict

M0 build spike, weeks 1-2, with the first go/no-go: **a 64-bit libtol builds on a modern toolchain and `Estimate` runs, else abort B**. M1 shim and cffi, weeks 3-5. M2 bridges, weeks 6-8. M3 pytest harness and the statsmodels cross-check, weeks 9-10. M4 packaging, weeks 11-13, with Windows and a capped macOS attempt. M5 Jupyter kernel, Pygments lexer, hooks, weeks 14-16. Risks, ranked: toolchain drift; the missing StdLib surface; object-lifetime semantics; process-global state; latent 64-bit bugs in the disabled contribs; Windows and macOS builds; the GPL; performance traps in dating compatibility. Verdict: a bridge. Its durable value is as an executable specification and a differential oracle for Option A, an exact legacy parser for Option C, and a loader that converts old `.oza` containers to parquet; it can be retired once the pure-Python library passes the differential suite on the user's own corpus.

## 7. Option C: a TOL-like language hosted on Python

### 7.1 What is confusing in TOL, from real code

A census of 799 `.tol` files (50,447 lines) in `tol_tests/tol` and `tol/stdlib` grounds the catalogue. Highlights: the mandatory type prefix is simultaneously a declaration, a cast and the overload selector (`tol/btol/bgrammar/graimp.cpp:732-735`); `[[ ]]` is rewritten by the filter and its declarations leak into the enclosing scope because only `{` opens a level (`graimp.cpp:650-654`); `Struct @Name` has four body spellings and registers the name as a parse-time type token; `_x`, `_.x` and `_this` encode visibility; `::` (3,462 uses) and `->` (472) are two spellings of member access; `?` is both the unknown value and a dummy argument; `<<` (2,924 uses) means four different things by grammar; `:` applies a polynomial to a series; `Real y = x` aliases and `Copy(Time)` is needed to snapshot (785 uses of `Copy(NError)` alone); scoping is dynamic; there are five load mechanisms, two of them at parse time; `Eval` on generated strings (158 uses) stands in for dictionaries; Spanish identifiers (`Diario` 182 uses); no `else`; typed lambdas in `For`/`EvalSet`/`Select`; 1-based positional result access such as `est1[3][4]`; errors do not unwind. For each, the design records keep, rename or replace with the proposed English form.

### 7.2 Three architectures and the recommendation

- **(i) Transpiler.** Do not reuse `tol/bparser` in-process (static tables, a 64 KB buffer, parse-time I/O and package loading, context-sensitive type registration). Generate a ~150-line `lark` grammar from the token tables in `scn.cpp:260-390`, with a two-pass reader for Struct/Class names, and keep the C++ parser only as an oracle through Option B. The semantic mapping is tabulated: typed declarations to annotated assignments, `[[ ]]` with hoisting to reproduce the leak, Structs to dataclasses, NameBlocks to namespace objects, Classes to classes, functions with emitted `return`, blocks-as-values through lifted closures (which preserves the corpus's dynamic-scope pattern), eager evaluation except inside Serie and TimeSet, value semantics with a warning on `:=` targets bound by aliasing, exceptions with an `NError` counter shim so migrated tests keep working, `include()` with provenance, `#Require` through importlib with census stubs, `s[k]` to `s[k-1]`, `Eval` through an `exec` shim. Of 858 distinct registered built-in names, 401 are ever called in the corpus and 175 in five or more files; roughly 90 of the 401 must come from the runtime, the rest are one-line delegations to numpy, scipy, Python builtins and the standard library.
- **(ii) A new DSL.** Python-like blocks, optional inferred types, implicit trailing-expression return, `if/elif/else` as statement and expression, `struct`/`class`/`namespace`, `NA`, `B` and `F` with `**`, `@` for application, calendar literals, `d"2004-01-12"` dates, zero-based indexing; compiled through lark to Python `ast` with source maps. Three side-by-sides (an ARIMA definition from `ARIMA/Missing/MissingEstimation.tol:12-42`, a NameBlock/Class from `_timer.tol` and `example.01.tol`, and a TimeSet/Serie manipulation from `bug_000079` and `bug_000426`) show that the plain Python API reads as well as the DSL for the distinctive core; the DSL's advantages concentrate on blocks, implicit return and if-expressions, which Python nearly has.
- **(iii) Embedded DSL in Python.** Everything distinctive fits Python operators: `1 - B**12`, `poly @ serie`, series arithmetic with NA propagation through `__array_ufunc__`, TimeSet set algebra, `with dating(monthly):`, fluent model builders. Lost only sugar: `^` for power, an NA-propagating `If`, self-naming objects, implicit last-expression blocks, the type-prefix overload selection and the declare-and-collect idiom.

**Recommendation.** Build (iii) first, because it is Option A's public API and costs nothing extra. Build the parser half of (i) next, because it powers a migration census, a Pygments lexer and a linter, and its transpiler half only as a best-effort aid for leaf code. Do not build (ii) until the embedded API has been used to write real models; otherwise the grammar is designed against imagined pain. A transpiler that promises to *run* legacy code is not viable: 52 of the 799 files require packages that are not here (39 for BysMcmc, 4 for MMS), the harness struct used by 487 files is external, the calendars are external, and 447 files are Latin-1.

### 7.3 Tooling, roadmap, verdict

Tooling a language layer enables: a Pygments lexer and a TextMate grammar for legacy `.tol` generated from the single token table; a `%%tol` Jupyter magic; a formatter mirroring `BParser::Unparse` with round-trip tests; a linter for the catalogued pitfalls; and the **TOL census**, a per-file migration report listing parse status, packages required and whether they exist, unsupported built-ins, external StdLib symbols, encoding and lint counts. Roadmap for one language-tooling engineer: M1 lexer, census and Pygments (2 weeks; all 799 files tokenised), M2 parser, AST and unparser (4 weeks; 95 percent of the corpus parses, round-trips are AST-equal), M3 transpile the kernel batteries (5 weeks; 70 percent of `kernel`, `SetAlgebra`, `BooleanAlgebra`, `Text`, `CreationAndCopy` tests pass), M4 DSL v0 and magic (4 weeks), M5 editor, linter and formatter (3 weeks). Tooling-only subset about 6 person-weeks, full option about 18. Verdict: not a standalone project; a tooling layer inside Option A with B as the differential oracle.

## 8. The recommended program: D-phased

The "totalizing" option is right in spirit and wrong as a budget. Built naively it triples the cost for the same value, because A already contains the API that C would add and the runtime that B would wrap. Sequenced, it becomes the best plan:

| Step | What | Weeks | Gate |
|---|---|---|---|
| 1 | **Option A, phases 0-2**: dates, TimeSet, named calendars, Serie graph, adapters, legacy readers, lag-operator algebra | 12 | priority 1 usable; calendar definitions confirmed by the user |
| 2 (parallel with 1) | **Option B spike M0**: 64-bit libtol on Linux, `tolcon -ndp` running `SimulArima` and `Estimate` | 2, capped | go: continue B to M1-M3 as an oracle (another ~8 weeks, interleaved); no-go: record goldens from a one-off Docker `tolsh` build and drop B |
| 3 (parallel) | **Option C M1**: token table, lexer, census, Pygments | 2 | the census tells you what in the 2013-2015 code base is worth porting |
| 4 | **Option A phases 3-4**: ARIMA, AIA, GARCH, adapters, differential tests against B if it passed | 12 | priority 2 complete; parity report vs statsmodels and, if available, vs the kernel |
| 5 | **Option A phase 5**: MMS-style model management, using any recovered MMS containers as fixtures through B's `.oza` loader | 6 | priority 3 |
| 6 | **Option A phase 6-7**: BSR language, Gibbs sampler, arviz, docs, packaging | 14 | priority 4; pytol 1.0 |
| 7 (optional) | **Option C M2-M3**: parser, best-effort transpiler for leaf code; DSL only on demonstrated pain | 9 | migration aid |

Total about 50-60 person-weeks for one engineer with B as an oracle, or about 12-15 calendar months; two engineers (one Python/statistics, one C++/tooling) compress it to roughly 8-9 months because steps 2, 3 and the B oracle run alongside A.

## 9. Third-party libraries

Kept as C/C++, only inside the Option B binding: GSL and CBLAS, OpenBLAS (BLAS/LAPACK/LAPACKE), CHOLMOD and SuiteSparse, FFTW3, Boost (thread, system, filesystem), ZipArchive, bzip2, zlib, libltdl, sparsehash headers, and the compiled-in alglib, kmlocal, clusterlib, optimal_bw and dcdflib. Nothing from the C++ tree is kept in the Python library; the two numeric hot spots (Levinson likelihood, AIA) are small enough for numba.

| Library | Classification | Reason |
|---|---|---|
| numpy, pandas, scipy | core | arrays, DatetimeIndex interop, `optimize.least_squares`, `signal.lfilter`, `sparse`, `stats` (replaces TOL's 28 distributions and 343 GSL wrappers) |
| statsmodels | optional, near-core | Ljung-Box, PACF map, SARIMAX parity engine, AR/VAR |
| arch | optional | GARCH family on residuals (TOL had none) |
| numba | optional | Levinson, AIA inner loops, truncated-normal Gibbs; pure numpy fallback kept |
| pyarrow, polars | optional | Arrow-native storage and `to_polars`/`from_polars`; fastparquet unnecessary |
| duckdb | optional | MMS catalogue and out-of-core panel queries |
| pydantic | optional (`mms` extra) | typed, validated model specs |
| scikit-sparse, arviz, lark | optional (`bayes` extra) | CHOLMOD factorisations, chain diagnostics and storage, `.bsr` parser |
| pandera | optional | dataset contracts (dating, range, no gaps); great expectations, cleanlab out |
| sktime, darts | optional adapters | estimator protocol, pipelines, backtests; their model zoos are not wrapped |
| pmdarima, statsforecast | optional | order-search backends for `auto_spec` only |
| hierarchicalforecast | optional | reconciliation of pytol forecasts; mlforecast, neuralforecast, skforecast out (overlap or foreign model families) |
| scikit-learn | optional | `BaseEstimator` compatibility, cross-validation utilities |
| optuna | optional | hyper-search over ARIMA orders and AIA parameters |
| ruptures | optional | structural-break alternative to AIA in reports; tsfresh, tslearn out |
| dask, ray | optional | batch runner for many MMS models (replaces `Tpa`); the series graph itself is per-model |
| mlflow, dvc | optional | experiment tracking and data versioning for the container |
| jax, pytorch, cupy, taichi | out (cupy later as an array backend) | no autodiff needed; series values are plain ndarrays users can hand to torch |
| prophet, gluonts, pytorch-forecasting, chronos, timesfm, timegpt, uni2ts | out | foreign model families; `to_pandas()` is the bridge |
| lightgbm, xgboost, catboost, autogluon, flaml, h2o, tabpfn, pytorch-tabular, tabnet, cuML, scikit-learn-intelex | out | tabular ML, reachable through the sktime and Nixtla exports |
| sympy, mpmath | out | the lag-polynomial algebra is numeric; sympy only for pretty-printing docs |
| numexpr, bottleneck | out | already used inside pandas |
| pyspark, daft, modin, bodo, vaex, datatable, cuDF, ibis | out | dataframe engines; not needed for per-model series graphs |
| geopandas | out | TOL has no spatial semantics |
| ydata-profiling, sweetviz, d-tale, lux | out | EDA on `to_pandas()` output |
| shap, lime, interpretml | out | explanatory-term decomposition is native to the model |

Adapter surface: `Serie.to_pandas/to_polars/to_arrow/to_xarray/to_darts/to_sktime`, `Serie.from_pandas(dating=...)`, `TimeSet.to_pandas_freq()`, `ArimaFit.to_statsmodels()`, `BsrModel.to_pymc()` (validation only), `fit.residuals.to_arch()`.

## 10. Answers to the five questions

1. **From the ground up or from the repository?** From the ground up in Python for the runtime, with the repository as an executable specification rather than mere inspiration: the TimeSet contract, series semantics, the AIA algorithm, the BSR grammar, the diagnostic bands, the token tables, 480 test drivers and the BSR design chapters are precise. The kernel can additionally serve as a differential oracle through a small binding for two or three years, if the two-week build spike passes.
2. **1-to-1 translation, wrappers, or extensions? How similar to rtol, vbtol and TolJava?** Semantic translation of the seven distinctive capability groups (about 40k C++ lines compress to 10-12k Python lines); replacement of the commodity two thirds by the ecosystem; no long-term wrappers. The binding option is the same lineage as rtol, vbtol and TolJava (text in, named objects in, values out) but with ref-counted handles, zero-copy buffers, parse-free calls and error deltas; those three are reusable as patterns only, since each has verified defects (a dangling pointer in rtol, a twelve-fold re-evaluation probe and wrong JNI descriptors in TolJava, a 32-bit callback in vbtol).
3. **Which third-party libraries stay as C/C++ and which become Python libraries?** Section 9. Native libraries stay only inside the binding; the library core is numpy, pandas and scipy, with statsmodels, arch, numba, scikit-sparse, arviz and lark as optional layers and sktime/darts/Nixtla as adapters. The foundation-model, tabular-ML and dataframe-engine families are out of scope and reachable through exports.
4. **What to add?** Typed, validated specs and exceptions instead of positional Sets and silent Unknowns; zero-based, named results; Arrow-native storage; vectorised and numba evaluation with structural dating compatibility; a probabilistic forecast API shared by the MLE and Bayesian engines; arviz diagnostics; hierarchical reconciliation; GARCH; dataset contracts; experiment tracking; Jupyter magics, a Pygments lexer, a linter and a migration census; an English API with Spanish aliases; bilingual docstrings preserved from the `I2()` help strings.
5. **The option set.** Section 1's table: D-phased, then A, then B, then C, with D-naive rejected.

## 11. Open decisions for the user

- **Named calendar definitions.** Confirm the intended semantics of `Weekly`, `Monthly`, `Quarterly`, `Yearly`, `Decenial`, `Fornigthly`, `Bimonthly`, `Thirdannual`, `HalfYearly` (anchor day, alignment with `Daily`), or recover `StdLib` from an old install.
- **Unknown representation.** NaN in float64 (TOL's own choice, fastest, zero-copy) versus `pd.NA` nullable dtypes (exact three-valued logic in pandas). Recommended: NaN internally, `pd.NA` on export.
- **Indexing.** Zero-based (recommended, since the audience is numpy/pandas users) versus TOL's one-based.
- **The binding's licence.** libtol, GSL and FFTW make the binding GPL-3.0; the pure-Python library can be licensed freely. If GPL is unacceptable even for an internal oracle, skip B and record goldens from a one-off container build.
- **Where to run the build spike.** WSL2 or Docker on the current machine, or a cloud runner; the spike needs GCC 12+ and the conda-forge or EPEL packages for SuiteSparse, GSL, FFTW, Boost, sparsehash and OpenBLAS.
- **Old material.** Any of the items listed in section 3 changes the plan for the better: MMS or BysMcmc containers turn the model-management and Bayesian phases from reconstruction into porting.

## Appendix: evidence index

| Topic | Files |
|---|---|
| Package ecosystem, MMS | `tol/stdlib/TolPackage/listA.oza`, `tolbase/lib/toltk/tolpkg.tcl:100-113`, `tolbase/lib/nbdbmanager/nbdbmanager.tcl:93-140`, `tol/CentOS5.4_Install.txt:342-364`, `tol_tests/tol/Bugzilla/bug_001608/runMms.tol`, `TolJava/TOLJava-Ejemplos/src/com/hcarralero/toljava/ejemplos/Ejemplo10.java`, `doc/general/mole-spec/*.odt`, `doc/modeling/BSR/BSR_P2_S5_DynHlm.lyx` |
| Embedding API | `tol/tol_init.h`, `tol/init.cpp:293-383, 496-1020, 1301-1319, 1527-1567`, `tol/btol/bgrammar/graimp.cpp:933-965`, `tol_bgencon.h:58-63, 706-747`, `syn.cpp:478-534`, `tol/bbasic/tol_bout.h`, `toltcl/generic/tolaccess.cpp`, `rtol/source/tolRlink.cpp`, `TolJava/TOLJavaJNI/toljavajni.cpp`, `tol/TestUseTol.cpp` |
| Capability inventory | `tol/btol/timeset_type/`, `serie_type/`, `polynomial_type/`, `ratio_type/`, `polmat_type/`, `bmodel/`, `vmatrix_type/`, `matrix_type/gibbssampler.cpp`, `tol/bmath/bstat/`, `tol/bmath/gsl_ext/`, `tol/stdlib/arima/`, `tol/contrib/` |
| Grammar and corpus | `tol/bparser/scn.cpp:260-390`, `par.cpp`, `tol_tests/tol/**/test.tol`, `tol/stdlib/**/*.tol` |
