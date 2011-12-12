/*
 * File:   TOLMachine.h
 * Author: hcarralero
 *
 * Created on November 14, 2010, 7:13 PM
 */

#include <string.h>
#include <stdlib.h>

#include "toljavajni.h"

/*
 * Definiciones requeridas para TOL
 */

#if defined(linux)
#define UNIX 1
#endif

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <vector>

#include <tol/tol_init.h>
#include <tol/tol_version_def.h>

#include <tol/tol_bout.h>
#include <tol/tol_bset.h>

#include <tol/tol_bgrammar.h>
#include <tol/tol_blanguag.h>

#include <tol/tol_btext.h>
#include <tol/tol_btxtgra.h>

#include <tol/tol_bdat.h>
#include <tol/tol_bdatgra.h>

#include <tol/tol_bdate.h>
#include <tol/tol_bdtegra.h>

#include <tol/tol_bcomplex.h>
#include <tol/tol_bcmpgra.h>

#include <tol/tol_bmatrix.h>
#include <tol/tol_bmatgra.h>

#include <tol/tol_bvmat.h>
#include <tol/tol_bvmatgra.h>

#include <tol/tol_bpolyn.h>
#include <tol/tol_bpolgra.h>

#include <tol/tol_bratio.h>
#include <tol/tol_bratgra.h>

#include <tol/tol_bcode.h>
#include <tol/tol_bcodgra.h>

#include <tol/tol_btmset.h>
#include <tol/tol_bsetgra.h>

#include <tol/tol_btmser.h>
#include <tol/tol_btsrgra.h>

#include <tol/tol_bset.h>
#include <tol/tol_bsetgra.h>

#include <tol/tol_bnameblock.h>

//Arreglo de mensajes generados por TOL
vector<char *> mensajes;

//Función utilitaria para borrar todos los mensajes almacenados
void borrarMensajes(void) {
    int t = mensajes.size();
    for (int i = 0; i < t; ++i) {
        char *mensaje = mensajes[i];
        free(mensaje);
    }
    mensajes.clear();
}

//Función utilitaria para obtener mensajes del intérprete de TOL
void guardarMensaje(const BText &mensaje) {
    int l = mensaje.Length();
    char *cmsg = (char *) malloc(l * (sizeof (char) + 1));
    strcpy(cmsg, mensaje.String());
    mensajes.push_back(cmsg);
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeInitTOL
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeInitTOL(JNIEnv *env, jobject) {
    //Inicialización del Kernel de TOL
    int lang = 1;
    char *vmode = NULL;
    InitTolKernel(lang, vmode);
    LoadInitLibrary(0);
    jstring res = env->NewStringUTF("Ok");
    return res;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeExecute
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeExecute(JNIEnv *env, jobject, jstring code) {
    const jchar *jcode = env->GetStringChars(code, NULL);
    jsize lcode = env->GetStringLength(code);
    char *ccode = (char *) malloc(sizeof (char) *(lcode + 1));
    for (int i = 0; i < lcode; ++i) {
        ccode[i] = jcode[i];
    }
    ccode[lcode] = '\0';
    borrarMensajes();
    BOut::PutHciWriter(guardarMensaje);
    BOut::PutLogHciWriter(guardarMensaje);
    TOLErrorNumber().GetValue() = 0;
    MultyEvaluate(ccode);
    int ne = TOLErrorNumber().GetValue();
    char cres[100] = "0";
    sprintf(cres, "%d", ne);
    int nm = mensajes.size();
    jobjectArray res = env->NewObjectArray(nm + 1, env->FindClass("Ljava/lang/String;"), NULL);
    jstring msj1 = env->NewStringUTF(cres);
    env->SetObjectArrayElement(res, 0, msj1);
    for (int i = 1; i <= nm; ++i) {
        jstring msj = env->NewStringUTF(mensajes[i - 1]);
        env->SetObjectArrayElement(res, i, msj);
    }
    borrarMensajes();
    return res;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeCallCode
 * Signature: (Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeCallCode(JNIEnv *env, jobject, jstring codeName, jobjectArray parameters) {
    const char *ccname = env->GetStringUTFChars(codeName, NULL);
    BSyntaxObject *bsoc = GraCode()->EvaluateExpr(ccname);
    if (bsoc == NULL) {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Nombre de variable Code NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    } else {
        BUserCode *bc = UCode(bsoc);
        if (bc == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("Error Interno de TOL");
            jobject e = env->NewObject(eclass, einit, 4, msg);
            return e;
        } else {
            BText expr = BText(bc->Name().String());
            expr += BText("(");
            int npar = env->GetArrayLength(parameters);
            char coma = 'N';
            for (int i = 0; i < npar; ++i) {
                jobject jopar = env->GetObjectArrayElement(parameters, i);
                if (jopar == NULL) {
                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                    jstring msg = env->NewStringUTF("Parámetro NO VÁLIDO (null)");
                    jobject e = env->NewObject(eclass, einit, 5, msg);
                    return e;
                } else {
                    jclass parclass = env->GetObjectClass(jopar);
                    jmethodID mtoString = env->GetMethodID(parclass, "toString", "()Ljava/lang/String;");
                    jstring jspar = (jstring) env->CallObjectMethod(jopar, mtoString);
                    const char *cpar = env->GetStringUTFChars(jspar, NULL);
                    if (coma == 'N') {
                        coma = 'S';
                    } else {
                        expr += BText(",");
                    }
                    expr += BText(cpar);
                }
            }
            expr += BText(");");
            BSyntaxObject * bsores = bc->Contens().Grammar()->EvaluateExpr(expr);
            if (bsores == NULL) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Error Interno de TOL (eval=NULL)");
                jobject e = env->NewObject(eclass, einit, 4, msg);
                return e;
            }
            BUserText *btxt = UText(bsores);
            if (btxt != NULL) {
                jclass ttclass = env->FindClass("Lcom/hcarralero/toljava/TOLText;");
                jmethodID ttinit = env->GetMethodID(ttclass, "<init>", "(Ljava/lang/String;)V");
                jstring tttext = env->NewStringUTF(btxt->Contens().String());
                jobject res = env->NewObject(ttclass, ttinit, tttext);
                return res;
            } else {
                BUserDat *br = UDat(bsores);
                if (br != NULL) {
                    jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLReal;");
                    jmethodID trinit = env->GetMethodID(trclass, "<init>", "(D)V");
                    double trvalue = br->Contens().GetValue();
                    jobject res = env->NewObject(trclass, trinit, trvalue);
                    return res;
                } else {
                    BUserDate *bd = UDate(bsores);
                    if (bd != NULL) {
                        jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                        jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                        BDate date = bd->Contens();
                        int year = date.Year();
                        int month = date.Month();
                        int day = date.Day();
                        int hour = date.Hour();
                        int minute = date.Minute();
                        double second = date.Second();
                        jobject res = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                        return res;
                    } else {
                        BUserCmp *bc = UCmp(bsores);
                        if (bc != NULL) {
                            jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLComplex;");
                            jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(DD)V");
                            double x = bc->Contens().X().GetValue();
                            double y = bc->Contens().Y().GetValue();
                            jobject res = env->NewObject(tcclass, tcinit, x, y);
                            return res;
                        } else {
                            BUserMat *bmat = UMat(bsores);
                            if (bmat != NULL) {
                                jclass tmclass = env->FindClass("Lcom/hcarralero/toljava/TOLMatrix;");
                                jmethodID tminit = NULL;
                                BMatrix<BDat> mat = bmat->Contens();
                                int rows = mat.Rows();
                                int columns = mat.Columns();
                                if (rows <= 0 || columns <= 0) {
                                    tminit = env->GetMethodID(tmclass, "<init>", "()V");
                                    jobject res = env->NewObject(tmclass, tminit);
                                    return res;
                                } else {
                                    tminit = env->GetMethodID(tmclass, "<init>", "([[D)V");
                                    BArray<BDat> vals = mat.Data();
                                    jclass adclass = env->FindClass("[D");
                                    jobjectArray tmvalues = env->NewObjectArray(rows, adclass, NULL);
                                    for (int r = 0; r < rows; ++r) {
                                        jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                        jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                        for (int c = 0; c < columns; ++c) {
                                            valuesRow[c] = vals[r * columns + c].Value();
                                        }
                                        env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                        env->SetObjectArrayElement(tmvalues, r, arrayRow);
                                    }
                                    jobject res = env->NewObject(tmclass, tminit, tmvalues);
                                    return res;
                                }
                            } else {
                                BUserVMat *bvmat = UVMat(bsores);
                                if (bvmat != NULL) {
                                    jclass tvmclass = env->FindClass("Lcom/hcarralero/toljava/TOLVMatrix;");
                                    jmethodID tvminit = NULL;
                                    BVMat vmat = bvmat->Contens();
                                    int rows = vmat.Rows();
                                    int columns = vmat.Columns();
                                    if (rows <= 0 || columns <= 0) {
                                        tvminit = env->GetMethodID(tvmclass, "<init>", "()V");
                                        jobject res = env->NewObject(tvmclass, tvminit);
                                        return res;
                                    } else {
                                        tvminit = env->GetMethodID(tvmclass, "<init>", "([[D)V");
                                        BMatrix<double> mat = vmat.GetDMat();
                                        BArray<double> vals = mat.Data();
                                        jclass adclass = env->FindClass("[D");
                                        jobjectArray tvmvalues = env->NewObjectArray(rows, adclass, NULL);
                                        for (int r = 0; r < rows; ++r) {
                                            jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                            jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                            for (int c = 0; c < columns; ++c) {
                                                valuesRow[c] = vals[r * columns + c];
                                            }
                                            env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                            env->SetObjectArrayElement(tvmvalues, r, arrayRow);
                                        }
                                        jobject res = env->NewObject(tvmclass, tvminit, tvmvalues);
                                        return res;
                                    }
                                } else {
                                    BUserPol *bp = UPol(bsores);
                                    if (bp != NULL) {
                                        jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                        jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                        BPolyn<BDat> pol = bp->Contens();
                                        int nt = pol.Size();
                                        jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                        jobject res = env->NewObject(tpclass, tpinit);
                                        for (int i = 0; i < nt; ++i) {
                                            BMonome<BDat> bm = pol[i];
                                            int var = 0;
                                            int grade = bm.Degree();
                                            if (grade < 0) {
                                                grade = -grade;
                                                var = 1;
                                            }
                                            double coef = bm.Coef().GetValue();
                                            env->CallVoidMethod(res, maddTerm, var, coef, grade);
                                        }
                                        return res;
                                    } else {
                                        BUserRat *br = URat(bsores);
                                        if (br != NULL) {
                                            BRational<BDat> rat = br->Contens();
                                            jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLRatio;");
                                            jmethodID trinit = env->GetMethodID(trclass, "<init>", "(Lcom/hcarralero/toljava/TOLPolyn;Lcom/hcarralero/toljava/TOLPolyn;)V");
                                            jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                            jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                            BPolyn<BDat> pnum = rat.Numerator();
                                            int nt = pnum.Size();
                                            jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                            jobject valueNum = env->NewObject(tpclass, tpinit);
                                            for (int i = 0; i < nt; ++i) {
                                                BMonome<BDat> bmnum = pnum[i];
                                                int var = 0;
                                                int grade = bmnum.Degree();
                                                if (grade < 0) {
                                                    grade = -grade;
                                                    var = 1;
                                                }
                                                double coef = bmnum.Coef().GetValue();
                                                env->CallVoidMethod(valueNum, maddTerm, var, coef, grade);
                                            }
                                            BPolyn<BDat> pden = rat.Denominator();
                                            nt = pden.Size();
                                            jobject valueDen = env->NewObject(tpclass, tpinit);
                                            for (int i = 0; i < nt; ++i) {
                                                BMonome<BDat> bmden = pden[i];
                                                int var = 0;
                                                int grade = bmden.Degree();
                                                if (grade < 0) {
                                                    grade = -grade;
                                                    var = 1;
                                                }
                                                double coef = bmden.Coef().GetValue();
                                                env->CallVoidMethod(valueDen, maddTerm, var, coef, grade);
                                            }
                                            jobject res = env->NewObject(trclass, trinit, valueNum, valueDen);
                                            return res;
                                        } else {
                                            BUserCode *bc = UCode(bsores);
                                            if (bc != NULL) {
                                                jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
                                                jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
                                                BCode cod = bc->Contens();
                                                jstring name = env->NewStringUTF(cod.Name().String());
                                                jstring info = env->NewStringUTF(cod.Info().String());
                                                jobject res = env->NewObject(tcclass, tcinit, name, info);
                                                return res;
                                            } else {
                                                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                jstring msg = env->NewStringUTF("Error Interno de TOL (res=NULL)");
                                                jobject e = env->NewObject(eclass, einit, 4, msg);
                                                return e;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetVariableValue
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetVariable(JNIEnv *env, jobject tm, jstring varName) {
    borrarMensajes();
    const char *cvname = env->GetStringUTFChars(varName, NULL);
    BSyntaxObject *bso = NULL;
    jobject varValue = NULL;
    jstring varDesc = NULL;
    bso = GraText()->EvaluateExpr(cvname);
    if (bso != NULL) {
        BUserText *btxt = UText(bso);
        if (btxt != NULL) {
            jclass ttclass = env->FindClass("Lcom/hcarralero/toljava/TOLText;");
            jmethodID ttinit = env->GetMethodID(ttclass, "<init>", "(Ljava/lang/String;)V");
            jstring ttvalue = env->NewStringUTF(btxt->Contens().String());
            varValue = env->NewObject(ttclass, ttinit, ttvalue);
        }
    } else {
        bso = GraReal()->EvaluateExpr(cvname);
        if (bso != NULL) {
            BUserDat *br = UDat(bso);
            if (br != NULL) {
                jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLReal;");
                jmethodID trinit = env->GetMethodID(trclass, "<init>", "(D)V");
                double trvalue = br->Contens().GetValue();
                varValue = env->NewObject(trclass, trinit, trvalue);
            }
        } else {
            bso = GraDate()->EvaluateExpr(cvname);
            if (bso != NULL) {
                BUserDate *bd = UDate(bso);
                if (bd != NULL) {
                    jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                    jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                    BDate date = bd->Contens();
                    int year = date.Year();
                    int month = date.Month();
                    int day = date.Day();
                    int hour = date.Hour();
                    int minute = date.Minute();
                    double second = date.Second();
                    varValue = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                }
            } else {
                bso = GraComplex()->EvaluateExpr(cvname);
                if (bso != NULL) {
                    BUserCmp *bc = UCmp(bso);
                    if (bc != NULL) {
                        jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLComplex;");
                        jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(DD)V");
                        double x = bc->Contens().X().GetValue();
                        double y = bc->Contens().Y().GetValue();
                        varValue = env->NewObject(tcclass, tcinit, x, y);
                    }
                } else {
                    bso = GraMatrix()->EvaluateExpr(cvname);
                    if (bso != NULL) {
                        BUserMat *bmat = UMat(bso);
                        if (bmat != NULL) {
                            jclass tmclass = env->FindClass("Lcom/hcarralero/toljava/TOLMatrix;");
                            jmethodID tminit = NULL;
                            BMatrix<BDat> mat = bmat->Contens();
                            int rows = mat.Rows();
                            int columns = mat.Columns();
                            if (rows <= 0 || columns <= 0) {
                                tminit = env->GetMethodID(tmclass, "<init>", "()V");
                                varValue = env->NewObject(tmclass, tminit);
                            } else {
                                tminit = env->GetMethodID(tmclass, "<init>", "([[D)V");
                                BArray<BDat> vals = mat.Data();
                                jclass adclass = env->FindClass("[D");
                                jobjectArray tmvalues = env->NewObjectArray(rows, adclass, NULL);
                                for (int r = 0; r < rows; ++r) {
                                    jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                    jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                    for (int c = 0; c < columns; ++c) {
                                        valuesRow[c] = vals[r * columns + c].Value();
                                    }
                                    env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                    env->SetObjectArrayElement(tmvalues, r, arrayRow);
                                }
                                varValue = env->NewObject(tmclass, tminit, tmvalues);
                            }
                        }
                    } else {
                        bso = GraVMatrix()->EvaluateExpr(cvname);
                        if (bso != NULL) {
                            BUserVMat *bvmat = UVMat(bso);
                            if (bvmat != NULL) {
                                jclass tvmclass = env->FindClass("Lcom/hcarralero/toljava/TOLVMatrix;");
                                jmethodID tvminit = NULL;
                                BVMat vmat = bvmat->Contens();
                                int rows = vmat.Rows();
                                int columns = vmat.Columns();
                                if (rows <= 0 || columns <= 0) {
                                    tvminit = env->GetMethodID(tvmclass, "<init>", "()V");
                                    varValue = env->NewObject(tvmclass, tvminit);
                                } else {
                                    tvminit = env->GetMethodID(tvmclass, "<init>", "([[D)V");
                                    BMatrix<double> mat = vmat.GetDMat();
                                    BArray<double> vals = mat.Data();
                                    jclass adclass = env->FindClass("[D");
                                    jobjectArray tvmvalues = env->NewObjectArray(rows, adclass, NULL);
                                    for (int r = 0; r < rows; ++r) {
                                        jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                        jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                        for (int c = 0; c < columns; ++c) {
                                            valuesRow[c] = vals[r * columns + c];
                                        }
                                        env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                        env->SetObjectArrayElement(tvmvalues, r, arrayRow);
                                    }
                                    varValue = env->NewObject(tvmclass, tvminit, tvmvalues);
                                }
                            }
                        } else {
                            bso = GraPolyn()->EvaluateExpr(cvname);
                            if (bso != NULL) {
                                BUserPol *bp = UPol(bso);
                                if (bp != NULL) {
                                    jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                    jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                    BPolyn<BDat> pol = bp->Contens();
                                    int nt = pol.Size();
                                    jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                    varValue = env->NewObject(tpclass, tpinit);
                                    for (int i = 0; i < nt; ++i) {
                                        BMonome<BDat> bm = pol[i];
                                        int var = 0;
                                        int grade = bm.Degree();
                                        if (grade < 0) {
                                            grade = -grade;
                                            var = 1;
                                        }
                                        double coef = bm.Coef().GetValue();
                                        env->CallVoidMethod(varValue, maddTerm, var, coef, grade);
                                    }
                                }
                            } else {
                                bso = GraRatio()->EvaluateExpr(cvname);
                                if (bso != NULL) {
                                    BUserRat *br = URat(bso);
                                    if (br != NULL) {
                                        BRational<BDat> rat = br->Contens();
                                        jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLRatio;");
                                        jmethodID trinit = env->GetMethodID(trclass, "<init>", "(Lcom/hcarralero/toljava/TOLPolyn;Lcom/hcarralero/toljava/TOLPolyn;)V");
                                        jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                        jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                        BPolyn<BDat> pnum = rat.Numerator();
                                        int nt = pnum.Size();
                                        jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                        jobject valueNum = env->NewObject(tpclass, tpinit);
                                        for (int i = 0; i < nt; ++i) {
                                            BMonome<BDat> bmnum = pnum[i];
                                            int var = 0;
                                            int grade = bmnum.Degree();
                                            if (grade < 0) {
                                                grade = -grade;
                                                var = 1;
                                            }
                                            double coef = bmnum.Coef().GetValue();
                                            env->CallVoidMethod(valueNum, maddTerm, var, coef, grade);
                                        }
                                        BPolyn<BDat> pden = rat.Denominator();
                                        nt = pden.Size();
                                        jobject valueDen = env->NewObject(tpclass, tpinit);
                                        for (int i = 0; i < nt; ++i) {
                                            BMonome<BDat> bmden = pden[i];
                                            int var = 0;
                                            int grade = bmden.Degree();
                                            if (grade < 0) {
                                                grade = -grade;
                                                var = 1;
                                            }
                                            double coef = bmden.Coef().GetValue();
                                            env->CallVoidMethod(valueDen, maddTerm, var, coef, grade);
                                        }
                                        varValue = env->NewObject(trclass, trinit, valueNum, valueDen);
                                    }
                                } else {
                                    bso = GraCode()->EvaluateExpr(cvname);
                                    if (bso != NULL) {
                                        BUserCode *bc = UCode(bso);
                                        if (bc != NULL) {
                                            jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
                                            jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
                                            BCode cod = bc->Contens();
                                            jstring name = env->NewStringUTF(cod.Name().String());
                                            jstring info = env->NewStringUTF(cod.Info().String());
                                            varValue = env->NewObject(tcclass, tcinit, name, info);
                                        }
                                    } else {
                                        bso = GraSerie()->EvaluateExpr(cvname);
                                        if (bso != NULL) {
                                            BUserTimeSerie *bs = Tsr(bso);
                                            if (bs != NULL) {
                                                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSerie;");
                                                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                jstring vn = env->NewStringUTF(cvname);
                                                varValue = env->NewObject(tsclass, tsinit, tm, vn);
                                            }
                                        } else {
                                            bso = GraTimeSet()->EvaluateExpr(cvname);
                                            if (bso != NULL) {
                                                BUserTimeSet *bt = Tms(bso);
                                                if (bt != NULL) {
                                                    jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRTimeSet;");
                                                    jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                    jstring vn = env->NewStringUTF(cvname);
                                                    varValue = env->NewObject(tsclass, tsinit, tm, vn);
                                                }
                                            } else {
                                                bso = GraSet()->EvaluateExpr(cvname);
                                                if (bso != NULL) {
                                                    BUserSet *bs = USet(bso);
                                                    if (bs != NULL) {
                                                        jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSet;");
                                                        jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                        jstring vn = env->NewStringUTF(cvname);
                                                        varValue = env->NewObject(tsclass, tsinit, tm, vn);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (varValue == NULL) {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    varDesc = env->NewStringUTF(bso->Description().String());
    jobject var = env->NewObject(varclass, varinit, varName, varValue, varDesc);
    return var;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeSetVariableValue
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSetVariableValue(JNIEnv *env, jobject, jstring varName, jobject varValue) {
    const char *cvname = env->GetStringUTFChars(varName, NULL);
    jclass valueclass = env->GetObjectClass(varValue);
    BSyntaxObject *bso = NULL;
    bso = GraText()->EvaluateExpr(cvname);
    if (bso != NULL) {
        BUserText *btxt = UText(bso);
        if (btxt != NULL) {
            jmethodID valuem = env->GetMethodID(valueclass, "isText", "()Z");
            jboolean bs = env->CallBooleanMethod(varValue, valuem);
            if (!bs) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Se intenta asignar un valor NO TEXTO a una VARIABLE TIPO Text");
                jobject e = env->NewObject(eclass, einit, 3, msg);
                return e;
            } else {
                jmethodID mgetText = env->GetMethodID(valueclass, "getText", "()Ljava/lang/String;");
                jstring strvalue = (jstring) env->CallObjectMethod(varValue, mgetText);
                const jchar *jvalue = env->GetStringChars(strvalue, NULL);
                jsize lvalue = env->GetStringLength(strvalue);
                char *cvalue = (char *) malloc(sizeof (char) *(lvalue + 1));
                for (int i = 0; i < lvalue; ++i) {
                    cvalue[i] = jvalue[i];
                }
                cvalue[lvalue] = '\0';
                btxt->Contens() = cvalue;
            }
        }
    } else {
        bso = GraReal()->EvaluateExpr(cvname);
        if (bso != NULL) {
            BUserDat *br = UDat(bso);
            if (br != NULL) {
                jmethodID valuem = env->GetMethodID(valueclass, "isReal", "()Z");
                jboolean bs = env->CallBooleanMethod(varValue, valuem);
                if (!bs) {
                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                    jstring msg = env->NewStringUTF("Se intenta asignar un valor NO REAL a una VARIABLE TIPO Real");
                    jobject e = env->NewObject(eclass, einit, 3, msg);
                    return e;
                } else {
                    jmethodID mgetValue = env->GetMethodID(valueclass, "getValue", "()D");
                    jdouble value = env->CallDoubleMethod(varValue, mgetValue);
                    br->Contens() = value;
                }
            }
        } else {
            bso = GraDate()->EvaluateExpr(cvname);
            if (bso != NULL) {
                BUserDate *bd = UDate(bso);
                if (bd != NULL) {
                    jmethodID valuem = env->GetMethodID(valueclass, "isDate", "()Z");
                    jboolean bs = env->CallBooleanMethod(varValue, valuem);
                    if (!bs) {
                        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                        jstring msg = env->NewStringUTF("Se intenta asignar un valor NO DATE a una VARIABLE TIPO Date");
                        jobject e = env->NewObject(eclass, einit, 3, msg);
                        return e;
                    } else {
                        jmethodID mgetYear = env->GetMethodID(valueclass, "getYear", "()I");
                        int year = env->CallIntMethod(varValue, mgetYear);
                        bd->Contens().PutYear(year);
                        jmethodID mgetMonth = env->GetMethodID(valueclass, "getMonth", "()I");
                        int month = env->CallIntMethod(varValue, mgetMonth);
                        bd->Contens().PutMonth(month);
                        jmethodID mgetDay = env->GetMethodID(valueclass, "getDay", "()I");
                        int day = env->CallIntMethod(varValue, mgetDay);
                        bd->Contens().PutDay(day);
                        jmethodID mgetHour = env->GetMethodID(valueclass, "getHour", "()I");
                        int hour = env->CallIntMethod(varValue, mgetHour);
                        bd->Contens().PutHour(hour);
                        jmethodID mgetMinute = env->GetMethodID(valueclass, "getMinute", "()I");
                        int minute = env->CallIntMethod(varValue, mgetMinute);
                        bd->Contens().PutMinute(minute);
                        jmethodID mgetSecond = env->GetMethodID(valueclass, "getSecond", "()D");
                        double second = env->CallDoubleMethod(varValue, mgetSecond);
                        bd->Contens().PutSecond(second);
                    }
                }
            } else {
                bso = GraComplex()->EvaluateExpr(cvname);
                if (bso != NULL) {
                    BUserCmp *bc = UCmp(bso);
                    if (bc != NULL) {
                        jmethodID valuem = env->GetMethodID(valueclass, "isComplex", "()Z");
                        jboolean bs = env->CallBooleanMethod(varValue, valuem);
                        if (!bs) {
                            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                            jstring msg = env->NewStringUTF("Se intenta asignar un valor NO COMPLEX a una VARIABLE TIPO Complex");
                            jobject e = env->NewObject(eclass, einit, 3, msg);
                            return e;
                        } else {
                            jmethodID mgetX = env->GetMethodID(valueclass, "getX", "()D");
                            double x = env->CallDoubleMethod(varValue, mgetX);
                            jmethodID mgetY = env->GetMethodID(valueclass, "getY", "()D");
                            double y = env->CallDoubleMethod(varValue, mgetY);
                            bc->Contens() = BComplex::XY(BDat(x), BDat(y));
                        }
                    }
                } else {
                    bso = GraMatrix()->EvaluateExpr(cvname);
                    if (bso != NULL) {
                        BUserMat *bm = UMat(bso);
                        if (bm != NULL) {
                            jmethodID valuem = env->GetMethodID(valueclass, "isMatrix", "()Z");
                            jboolean bs = env->CallBooleanMethod(varValue, valuem);
                            if (!bs) {
                                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                jstring msg = env->NewStringUTF("Se intenta asignar un valor NO MATRIX a una VARIABLE TIPO Matrix");
                                jobject e = env->NewObject(eclass, einit, 3, msg);
                                return e;
                            } else {
                                jmethodID mgetRows = env->GetMethodID(valueclass, "getRows", "()I");
                                jint rows = env->CallIntMethod(varValue, mgetRows);
                                jmethodID mgetColumns = env->GetMethodID(valueclass, "getColumns", "()I");
                                jint columns = env->CallIntMethod(varValue, mgetColumns);
                                BDat *values = (BDat *) malloc(sizeof (BDat) * rows * columns);
                                jmethodID mgetValues = env->GetMethodID(valueclass, "getValues", "()[[D");
                                jobjectArray matValues = (jobjectArray) env->CallObjectMethod(varValue, mgetValues);
                                for (int i = 0; i < rows; ++i) {
                                    jdoubleArray rowArray = (jdoubleArray) env->GetObjectArrayElement(matValues, i);
                                    jdouble *rowValues = env->GetDoubleArrayElements(rowArray, NULL);
                                    for (int j = 0; j < columns; ++j) {
                                        values[i * columns + j] = rowValues[j];
                                    }
                                }
                                bm->Contens() = BMatrix<BDat > (rows, columns, values);
                            }
                        }
                    } else {
                        bso = GraVMatrix()->EvaluateExpr(cvname);
                        if (bso != NULL) {
                            BUserVMat *bvm = UVMat(bso);
                            if (bvm != NULL) {
                                jmethodID valuem = env->GetMethodID(valueclass, "isVMatrix", "()Z");
                                jboolean bs = env->CallBooleanMethod(varValue, valuem);
                                if (!bs) {
                                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                    jstring msg = env->NewStringUTF("Se intenta asignar un valor NO VMATRIX a una VARIABLE TIPO VMatrix");
                                    jobject e = env->NewObject(eclass, einit, 3, msg);
                                    return e;
                                } else {
                                    jmethodID mgetRows = env->GetMethodID(valueclass, "getRows", "()I");
                                    jint rows = env->CallIntMethod(varValue, mgetRows);
                                    jmethodID mgetColumns = env->GetMethodID(valueclass, "getColumns", "()I");
                                    jint columns = env->CallIntMethod(varValue, mgetColumns);
                                    jmethodID mgetValues = env->GetMethodID(valueclass, "getValues", "()[[D");
                                    jobjectArray matValues = (jobjectArray) env->CallObjectMethod(varValue, mgetValues);
                                    for (int i = 0; i < rows; ++i) {
                                        jdoubleArray rowArray = (jdoubleArray) env->GetObjectArrayElement(matValues, i);
                                        jdouble *rowValues = env->GetDoubleArrayElements(rowArray, NULL);
                                        for (int j = 0; j < columns; ++j) {
                                            bvm->Contens().PutCell(i, j, rowValues[j]);
                                        }
                                    }
                                }
                            }
                        } else {
                            bso = GraPolyn()->EvaluateExpr(cvname);
                            if (bso != NULL) {
                                BUserPol *bp = UPol(bso);
                                if (bp != NULL) {
                                    jmethodID valuem = env->GetMethodID(valueclass, "isPolyn", "()Z");
                                    jboolean bs = env->CallBooleanMethod(varValue, valuem);
                                    if (!bs) {
                                        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                        jstring msg = env->NewStringUTF("Se intenta asignar un valor NO POLYN a una VARIABLE TIPO Polyn");
                                        jobject e = env->NewObject(eclass, einit, 3, msg);
                                        return e;
                                    } else {
                                        jmethodID mnterms = env->GetMethodID(valueclass, "getNterms", "()I");
                                        jint size = env->CallIntMethod(varValue, mnterms);
                                        BMonome<BDat> *terms = (BMonome<BDat> *)malloc(sizeof (BMonome<BDat>) * size);
                                        jmethodID mgetTerm = env->GetMethodID(valueclass, "getTerm", "()Lcom.hcarralero.toljava.TOLPolynTerm;");
                                        for (int i = 0; i < size; ++i) {
                                            jobject term = env->CallObjectMethod(varValue, mgetTerm, i);
                                            jclass termclass = env->FindClass("Lcom.hcarralero.toljava.TOLPolynTerm;");
                                            jmethodID mgetCoef = env->GetMethodID(termclass, "getCoef", "()D");
                                            double coef = env->CallDoubleMethod(term, mgetCoef);
                                            jmethodID mgetVar = env->GetMethodID(termclass, "getVar", "()I");
                                            int var = env->CallIntMethod(term, mgetVar);
                                            jmethodID mgetGrade = env->GetMethodID(termclass, "getGrade", "()I");
                                            int grade = env->CallIntMethod(term, mgetGrade);
                                            if (var == 1) {
                                                grade = -grade;
                                            }
                                            terms[i] = BMonome<BDat > (coef, grade);
                                        }
                                        bp->Contens() = BPolyn<BDat > (size, terms);
                                    }
                                }
                            } else {
                                bso = GraRatio()->EvaluateExpr(cvname);
                                if (bso != NULL) {
                                    BUserRat *br = URat(bso);
                                    if (br != NULL) {
                                        jmethodID valuem = env->GetMethodID(valueclass, "isRatio", "()Z");
                                        jboolean bs = env->CallBooleanMethod(varValue, valuem);
                                        if (!bs) {
                                            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                            jstring msg = env->NewStringUTF("Se intenta asignar un valor NO RATIO a una VARIABLE TIPO Ratio");
                                            jobject e = env->NewObject(eclass, einit, 3, msg);
                                            return e;
                                        } else {
                                            jmethodID mgetNumerator = env->GetMethodID(valueclass, "getNumerator", "()Lcom/hcarralero/toljava/TOLPolyn;");
                                            jobject pnum = env->CallObjectMethod(varValue, mgetNumerator);
                                            jclass pclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                            jmethodID mnterms = env->GetMethodID(pclass, "getNterms", "()I");
                                            jint size = env->CallIntMethod(pnum, mnterms);
                                            BMonome<BDat> *pnumterms = (BMonome<BDat> *)malloc(sizeof (BMonome<BDat>) * size);
                                            jmethodID mgetTerm = env->GetMethodID(pclass, "getTerm", "()Lcom.hcarralero.toljava.TOLPolynTerm;");
                                            for (int i = 0; i < size; ++i) {
                                                jobject term = env->CallObjectMethod(pnum, mgetTerm, i);
                                                jclass termclass = env->FindClass("Lcom.hcarralero.toljava.TOLPolynTerm;");
                                                jmethodID mgetCoef = env->GetMethodID(termclass, "getCoef", "()D");
                                                double coef = env->CallDoubleMethod(term, mgetCoef);
                                                jmethodID mgetVar = env->GetMethodID(termclass, "getVar", "()I");
                                                int var = env->CallIntMethod(term, mgetVar);
                                                jmethodID mgetGrade = env->GetMethodID(termclass, "getGrade", "()I");
                                                int grade = env->CallIntMethod(term, mgetGrade);
                                                if (var == 1) {
                                                    grade = -grade;
                                                }
                                                pnumterms[i] = BMonome<BDat > (coef, grade);
                                            }
                                            BPolyn<BDat> num = BPolyn<BDat > (size, pnumterms);
                                            br->Contens().PutNumerator(num);
                                            jmethodID mgetDenominator = env->GetMethodID(valueclass, "getDenominator", "()Lcom/hcarralero/toljava/TOLPolyn;");
                                            jobject pden = env->CallObjectMethod(varValue, mgetDenominator);
                                            size = env->CallIntMethod(pden, mnterms);
                                            BMonome<BDat> *pdenterms = (BMonome<BDat> *)malloc(sizeof (BMonome<BDat>) * size);
                                            for (int i = 0; i < size; ++i) {
                                                jobject term = env->CallObjectMethod(pden, mgetTerm, i);
                                                jclass termclass = env->FindClass("Lcom.hcarralero.toljava.TOLPolynTerm;");
                                                jmethodID mgetCoef = env->GetMethodID(termclass, "getCoef", "()D");
                                                double coef = env->CallDoubleMethod(term, mgetCoef);
                                                jmethodID mgetVar = env->GetMethodID(termclass, "getVar", "()I");
                                                int var = env->CallIntMethod(term, mgetVar);
                                                jmethodID mgetGrade = env->GetMethodID(termclass, "getGrade", "()I");
                                                int grade = env->CallIntMethod(term, mgetGrade);
                                                if (var == 1) {
                                                    grade = -grade;
                                                }
                                                pdenterms[i] = BMonome<BDat > (coef, grade);
                                            }
                                            BPolyn<BDat> den = BPolyn<BDat > (size, pdenterms);
                                            br->Contens().PutDenominator(den);
                                        }
                                    }
                                } else {
                                    bso = GraCode()->EvaluateExpr(cvname);
                                    if (bso != NULL) {
                                        BUserCode *bc = UCode(bso);
                                        if (bc != NULL) {
                                            jmethodID valuem = env->GetMethodID(valueclass, "isCode", "()Z");
                                            jboolean bs = env->CallBooleanMethod(varValue, valuem);
                                            if (!bs) {
                                                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                jstring msg = env->NewStringUTF("Se intenta asignar un valor NO CODE a una VARIABLE TIPO Code");
                                                jobject e = env->NewObject(eclass, einit, 3, msg);
                                                return e;
                                            } else {
                                                jmethodID mfunction = env->GetMethodID(valueclass, "getFunction", "()Ljava/lang/String;");
                                                jstring jsnameNuevo = (jstring) env->CallObjectMethod(varValue, mfunction);
                                                const jchar *jcnameNuevo = env->GetStringChars(jsnameNuevo, NULL);
                                                jsize lnameNuevo = env->GetStringLength(jsnameNuevo);
                                                char *cnameNuevo = (char *) malloc(sizeof (char) *(lnameNuevo + 1));
                                                for (int i = 0; i < lnameNuevo; ++i) {
                                                    cnameNuevo[i] = jcnameNuevo[i];
                                                }
                                                cnameNuevo[lnameNuevo] = '\0';
                                                BSyntaxObject *bsoNuevo = GraCode()->EvaluateExpr(cnameNuevo);
                                                if (bsoNuevo == NULL) {
                                                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                    jstring msg = env->NewStringUTF("Nombre de Variable o Función NO ENCONTRADA.");
                                                    jobject e = env->NewObject(eclass, einit, 2, msg);
                                                    return e;
                                                } else {
                                                    BUserCode *bcNuevo = UCode(bsoNuevo);
                                                    bc->PutContens(bcNuevo->Contens());
                                                }
                                            }
                                        }
                                    } else {
                                        bso = GraTimeSet()->EvaluateExpr(cvname);
                                        if (bso != NULL) {
                                            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                            jstring msg = env->NewStringUTF("No se puede cambiar el valor de una variable tipo Set");
                                            jobject e = env->NewObject(eclass, einit, 3, msg);
                                            return e;
                                        } else {
                                            bso = GraSerie()->EvaluateExpr(cvname);
                                            if (bso != NULL) {
                                                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                jstring msg = env->NewStringUTF("No se puede cambiar el valor de una variable tipo Serie");
                                                jobject e = env->NewObject(eclass, einit, 3, msg);
                                                return e;
                                            } else {
                                                bso = GraSet()->EvaluateExpr(cvname);
                                                if (bso != NULL) {
                                                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                    jstring msg = env->NewStringUTF("No se puede cambiar el valor de una variable tipo Set");
                                                    jobject e = env->NewObject(eclass, einit, 3, msg);
                                                    return e;
                                                } else {
                                                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                                                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                                                    jstring msg = env->NewStringUTF("Variable NO ENCONTRADA: <<varName>>");
                                                    jobject e = env->NewObject(eclass, einit, 2, msg);
                                                    return e;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetTextVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetTextVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraText()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserText *btxt = UText(bso);
            if (btxt != NULL) {
                jclass ttclass = env->FindClass("Lcom/hcarralero/toljava/TOLText;");
                jmethodID ttinit = env->GetMethodID(ttclass, "<init>", "(Ljava/lang/String;)V");
                jstring ttvalue = env->NewStringUTF(btxt->Contens().String());
                varvalue = env->NewObject(ttclass, ttinit, ttvalue);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetRealVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetRealVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraReal()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserDat *br = UDat(bso);
            if (br != NULL) {
                jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLReal;");
                jmethodID trinit = env->GetMethodID(trclass, "<init>", "(D)V");
                double trvalue = br->Contens().GetValue();
                varvalue = env->NewObject(trclass, trinit, trvalue);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetDateVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetDateVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraDate()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserDate *bd = UDate(bso);
            if (bd != NULL) {
                jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                BDate date = bd->Contens();
                int year = date.Year();
                int month = date.Month();
                int day = date.Day();
                int hour = date.Hour();
                int minute = date.Minute();
                double second = date.Second();
                varvalue = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetComplexVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetComplexVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraComplex()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserCmp *bc = UCmp(bso);
            if (bc != NULL) {
                jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLComplex;");
                jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(DD)V");
                double x = bc->Contens().X().GetValue();
                double y = bc->Contens().Y().GetValue();
                varvalue = env->NewObject(tcclass, tcinit, x, y);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetMatrixVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetMatrixVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraMatrix()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserMat *bmat = UMat(bso);
            if (bmat != NULL) {
                jclass tmclass = env->FindClass("Lcom/hcarralero/toljava/TOLMatrix;");
                jmethodID tminit = NULL;
                BMatrix<BDat> mat = bmat->Contens();
                int rows = mat.Rows();
                int columns = mat.Columns();
                if (rows <= 0 || columns <= 0) {
                    tminit = env->GetMethodID(tmclass, "<init>", "()V");
                    varvalue = env->NewObject(tmclass, tminit);
                } else {
                    tminit = env->GetMethodID(tmclass, "<init>", "([[D)V");
                    BArray<BDat> vals = mat.Data();
                    jclass adclass = env->FindClass("[D");
                    jobjectArray tmvalues = env->NewObjectArray(rows, adclass, NULL);
                    for (int r = 0; r < rows; ++r) {
                        jdoubleArray arrayRow = env->NewDoubleArray(columns);
                        jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                        for (int c = 0; c < columns; ++c) {
                            valuesRow[c] = vals[r * columns + c].Value();
                        }
                        env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                        env->SetObjectArrayElement(tmvalues, r, arrayRow);
                    }
                    varvalue = env->NewObject(tmclass, tminit, tmvalues);
                }
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetVMatrixVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetVMatrixVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraVMatrix()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserVMat *bvmat = UVMat(bso);
            if (bvmat != NULL) {
                jclass tvmclass = env->FindClass("Lcom/hcarralero/toljava/TOLVMatrix;");
                jmethodID tvminit = NULL;
                BVMat vmat = bvmat->Contens();
                int rows = vmat.Rows();
                int columns = vmat.Columns();
                if (rows <= 0 || columns <= 0) {
                    tvminit = env->GetMethodID(tvmclass, "<init>", "()V");
                    varvalue = env->NewObject(tvmclass, tvminit);
                } else {
                    tvminit = env->GetMethodID(tvmclass, "<init>", "([[D)V");
                    BMatrix<double> mat = vmat.GetDMat();
                    BArray<double> vals = mat.Data();
                    jclass adclass = env->FindClass("[D");
                    jobjectArray tvmvalues = env->NewObjectArray(rows, adclass, NULL);
                    for (int r = 0; r < rows; ++r) {
                        jdoubleArray arrayRow = env->NewDoubleArray(columns);
                        jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                        for (int c = 0; c < columns; ++c) {
                            valuesRow[c] = vals[r * columns + c];
                        }
                        env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                        env->SetObjectArrayElement(tvmvalues, r, arrayRow);
                    }
                    varvalue = env->NewObject(tvmclass, tvminit, tvmvalues);
                }
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetPolynVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetPolynVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraPolyn()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserPol *bp = UPol(bso);
            if (bp != NULL) {
                jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                BPolyn<BDat> pol = bp->Contens();
                int nt = pol.Size();
                jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                varvalue = env->NewObject(tpclass, tpinit);
                for (int i = 0; i < nt; ++i) {
                    BMonome<BDat> bm = pol[i];
                    int var = 0;
                    int grade = bm.Degree();
                    if (grade < 0) {
                        grade = -grade;
                        var = 1;
                    }
                    double coef = bm.Coef().GetValue();
                    env->CallVoidMethod(varvalue, maddTerm, var, coef, grade);
                }
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeRatioVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetRatioVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraRatio()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserRat *br = URat(bso);
            if (br != NULL) {
                BRational<BDat> rat = br->Contens();
                jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLRatio;");
                jmethodID trinit = env->GetMethodID(trclass, "<init>", "(Lcom/hcarralero/toljava/TOLPolyn;Lcom/hcarralero/toljava/TOLPolyn;)V");
                jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                BPolyn<BDat> pnum = rat.Numerator();
                int nt = pnum.Size();
                jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                jobject valueNum = env->NewObject(tpclass, tpinit);
                for (int i = 0; i < nt; ++i) {
                    BMonome<BDat> bmnum = pnum[i];
                    int var = 0;
                    int grade = bmnum.Degree();
                    if (grade < 0) {
                        grade = -grade;
                        var = 1;
                    }
                    double coef = bmnum.Coef().GetValue();
                    env->CallVoidMethod(valueNum, maddTerm, var, coef, grade);
                }
                BPolyn<BDat> pden = rat.Denominator();
                nt = pden.Size();
                jobject valueDen = env->NewObject(tpclass, tpinit);
                for (int i = 0; i < nt; ++i) {
                    BMonome<BDat> bmden = pden[i];
                    int var = 0;
                    int grade = bmden.Degree();
                    if (grade < 0) {
                        grade = -grade;
                        var = 1;
                    }
                    double coef = bmden.Coef().GetValue();
                    env->CallVoidMethod(valueDen, maddTerm, var, coef, grade);
                }
                varvalue = env->NewObject(trclass, trinit, valueNum, valueDen);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetCodeVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetCodeVariables(JNIEnv *env, jobject) {
    borrarMensajes();
    BList *lv = GraCode()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    //    BList *lft = GraText()->GetOperators();
    //    BSet sft;
    //    sft.RobStruct(lft, NIL, BSet::Generic);
    //    int nft = sft.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    int i = 0;
    for (int v = 0; v < nv; ++v) {
        BSyntaxObject *bso = sv[v + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserCode *bc = UCode(bso);
            if (bc != NULL) {
                jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
                jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
                BCode cod = bc->Contens();
                jstring name = env->NewStringUTF(cod.Name().String());
                jstring info = env->NewStringUTF(cod.Info().String());
                varvalue = env->NewObject(tcclass, tcinit, name, info);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
        ++i;
    }
    //    for (int ft = 0; ft < nft; ++ft) {
    //        jobject var= NULL;
    //        BSyntaxObject *bso = sft[ft + 1];
    //        if (bso != NULL) {
    //            jstring varname = env->NewStringUTF(bso->Name().String());
    //            jobject vardesc = env->NewStringUTF(bso->Description().String());
    //            jobject varvalue = NULL;
    //            bso= bso->CopyContens();
    //            BUserCode *bc = UCode(bso);
    //            if (bc != NULL) {
    //                jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
    //                jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    //                BCode cod = bc->Contens();
    //                jstring name = env->NewStringUTF(cod.Name().String());
    //                jstring info = env->NewStringUTF(cod.Dump().String());
    //                varvalue = env->NewObject(tcclass, tcinit, name, info);
    //            }
    //            var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
    //        }
    //        env->SetObjectArrayElement(vars, i, var);
    //        ++i;
    //    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSerieVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetSerieVariables(JNIEnv *env, jobject tm) {
    borrarMensajes();
    BList *lv = GraSerie()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserTimeSerie *br = Tsr(bso);
            if (br != NULL) {
                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSerie;");
                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                varvalue = env->NewObject(tsclass, tsinit, tm, varname);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetTimeSetVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetTimeSetVariables(JNIEnv *env, jobject tm) {
    borrarMensajes();
    BList *lv = ::GraTimeSet()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserTimeSet *bt = Tms(bso);
            if (bt != NULL) {
                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRTimeSet;");
                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                varvalue = env->NewObject(tsclass, tsinit, tm, varname);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSetVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetRSetVariables(JNIEnv *env, jobject tm){
    borrarMensajes();
    BList *lv = GraSet()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserSet *bs = USet(bso);
            if (bs != NULL) {
                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSet;");
                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                varvalue = env->NewObject(tsclass, tsinit, tm, varname);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSetVariables
 * Signature: ()[Lcom/hcarralero/toljava/TOLVariable;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetSetVariables(JNIEnv *env, jobject tm){
    borrarMensajes();
    BList *lv = GraSet()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass varclass = env->FindClass("Lcom/hcarralero/toljava/TOLVariable;");
    jobjectArray vars = env->NewObjectArray(nv, varclass, NULL);
    jmethodID varinit = env->GetMethodID(varclass, "<init>", "(Ljava/lang/String;Lcom/hcarralero/toljava/TOLObject;Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        jstring varname = env->NewStringUTF(bso->Name().String());
        jobject vardesc = env->NewStringUTF(bso->Description().String());
        jobject varvalue = NULL;
        if (bso != NULL) {
            BUserSet *bs = USet(bso);
            if (bs != NULL) {
                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLSet;");
                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                varvalue = env->NewObject(tsclass, tsinit, tm, varname);
            }
        }
        jobject var = env->NewObject(varclass, varinit, varname, varvalue, vardesc);
        env->SetObjectArrayElement(vars, i, var);
    }
    return vars;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetNameBlocks
 * Signature: ()[Lcom/hcarralero/toljava/TOLNameBlock;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeGetNameBlocks(JNIEnv *env, jobject tm) {
    borrarMensajes();
    BList *lv = GraNameBlock()->GetVariables();
    BSet sv;
    sv.RobStruct(lv, NIL, BSet::Generic);
    int nv = sv.Card();
    jclass nbclass = env->FindClass("Lcom/hcarralero/toljava/TOLNameBlock;");
    jobjectArray nbs = env->NewObjectArray(nv, nbclass, NULL);
    jmethodID nbinit = env->GetMethodID(nbclass, "<init>", "(Ljava/lang/String;)V");
    for (int i = 0; i < nv; ++i) {
        BSyntaxObject *bso = sv[i + 1];
        if (bso != NULL) {
            BUserNameBlock *bn = UNameBlock(bso);
            if (bn != NULL) {
                jobject nb = env->NewObject(nbclass, nbinit, bn->Contens().Name().String());
                env->SetObjectArrayElement(nbs, i, nb);
            }
        }
    }
    return nbs;
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSerieValue
 * Signature: (Ljava/lang/String;Ljava/util/TOLDate;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetValue__Ljava_lang_String_2Lcom_hcarralero_toljava_TOLDate_2(JNIEnv *env, jobject, jstring varname, jobject date)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("NO SE QUË PINGA PASÓ");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
            jmethodID mgetYear = env->GetMethodID(tdclass, "getYear", "()I");
            int year = env->CallIntMethod(date, mgetYear);
            jmethodID mgetMonth = env->GetMethodID(tdclass, "getMonth", "()I");
            int month = env->CallIntMethod(date, mgetMonth);
            jmethodID mgetDay = env->GetMethodID(tdclass, "getDay", "()I");
            int day = env->CallIntMethod(date, mgetDay);
            jmethodID mgetHour = env->GetMethodID(tdclass, "getHour", "()I");
            int hour = env->CallIntMethod(date, mgetHour);
            jmethodID mgetMinute = env->GetMethodID(tdclass, "getMinute", "()I");
            int minute = env->CallIntMethod(date, mgetMinute);
            jmethodID mgetSecond = env->GetMethodID(tdclass, "getSecond", "()D");
            double second = env->CallDoubleMethod(date, mgetSecond);
            BDate d;
            d.PutDate(year, month, day, hour, minute, second);
            int index = bs->GetIndex(d);
            if (index < 0) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Fecha NO VÁLIDA");
                jobject e = env->NewObject(eclass, einit, 6, msg);
                return e;
            } else {
                jclass dclass = env->FindClass("Ljava/lang/Double;");
                jmethodID dinit = env->GetMethodID(dclass, "<init>", "(D)V");
                jobject value = env->NewObject(dclass, dinit, bs->GetDat(d).Value());
                return value;
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA: <<varName>>");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSerieValue
 * Signature: (Ljava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetValue__Ljava_lang_String_2I(JNIEnv *env, jobject, jstring varname, jint index) {
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("NO SE QUË PINGA PASÓ");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            if (index < 0) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Índice NO VÁLIDO");
                jobject e = env->NewObject(eclass, einit, 7, msg);
                return e;
            } else {
                BDate d = bs->IndexToDate(index);
                jclass dclass = env->FindClass("Ljava/lang/Double;");
                jmethodID dinit = env->GetMethodID(dclass, "<init>", "(D)V");
                jobject value = env->NewObject(dclass, dinit, bs->GetDat(d).Value());
                return value;
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA: <<varName>>");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSerieItems
 * Signature: (Ljava/lang/String;Lcom/hcarralero/toljava/TOLDate;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetItems__Ljava_lang_String_2Lcom_hcarralero_toljava_TOLDate_2I(JNIEnv *env, jobject, jstring varname, jobject date, jint size) {
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("NO SE QUË PINGA PASÓ");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            jobjectArray es = env->NewObjectArray(1, eclass, NULL);
            env->SetObjectArrayElement(es, 0, e);
            return es;
        } else {
            jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
            jmethodID mgetYear = env->GetMethodID(tdclass, "getYear", "()I");
            int year = env->CallIntMethod(date, mgetYear);
            jmethodID mgetMonth = env->GetMethodID(tdclass, "getMonth", "()I");
            int month = env->CallIntMethod(date, mgetMonth);
            jmethodID mgetDay = env->GetMethodID(tdclass, "getDay", "()I");
            int day = env->CallIntMethod(date, mgetDay);
            jmethodID mgetHour = env->GetMethodID(tdclass, "getHour", "()I");
            int hour = env->CallIntMethod(date, mgetHour);
            jmethodID mgetMinute = env->GetMethodID(tdclass, "getMinute", "()I");
            int minute = env->CallIntMethod(date, mgetMinute);
            jmethodID mgetSecond = env->GetMethodID(tdclass, "getSecond", "()D");
            double second = env->CallDoubleMethod(date, mgetSecond);
            BDate d;
            d.PutDate(year, month, day, hour, minute, second);
            int index = bs->GetIndex(d);
            if (index < 0) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Fecha NO VÁLIDA");
                jobject e = env->NewObject(eclass, einit, 5, msg);
                jobjectArray es = env->NewObjectArray(1, eclass, NULL);
                env->SetObjectArrayElement(es, 0, e);
                return es;
            } else {
                int l = bs->GetLength();
                if (index + size < l) {
                    l = index + size;
                }
                jclass tsiclass = env->FindClass("Lcom/hcarralero/toljava/TSItem;");
                jobjectArray tsis = env->NewObjectArray(l - index, tsiclass, NULL);
                jmethodID tsiinit = env->GetMethodID(tsiclass, "<init>", "(Lcom/hcarralero/toljava/TOLDate;D)V");
                for (int i = index; i < l; ++i) {
                    BDate date = bs->IndexToDate(i);
                    int year = date.Year();
                    int month = date.Month();
                    int day = date.Day();
                    int hour = date.Hour();
                    int minute = date.Minute();
                    double second = date.Second();
                    jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                    jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                    jobject tsidate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                    double tsivalue = bs->GetDat(date).Value();
                    jobject tsi = env->NewObject(tsiclass, tsiinit, tsidate, tsivalue);
                    env->SetObjectArrayElement(tsis, i - index, tsi);
                }
                return tsis;
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        jobjectArray es = env->NewObjectArray(1, eclass, NULL);
        env->SetObjectArrayElement(es, 0, e);
        return es;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetSerieItems
 * Signature: (Ljava/lang/String;II)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetItems__Ljava_lang_String_2II(JNIEnv *env, jobject, jstring varname, jint index, jint size) {
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        if (index < 0) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("Índice NO VÁLIDO");
            jobject e = env->NewObject(eclass, einit, 5, msg);
            jobjectArray es = env->NewObjectArray(1, eclass, NULL);
            env->SetObjectArrayElement(es, 0, e);
            return es;
        } else {
            int l = bs->GetLength();
            if (index + size < l) {
                l = index + size;
            }
            jclass tsiclass = env->FindClass("Lcom/hcarralero/toljava/TSItem;");
            jobjectArray tsis = env->NewObjectArray(l - index, tsiclass, NULL);
            jmethodID tsiinit = env->GetMethodID(tsiclass, "<init>", "(Lcom/hcarralero/toljava/TOLDate;D)V");
            for (int i = index; i < l; ++i) {
                BDate date = bs->IndexToDate(i);
                int year = date.Year();
                int month = date.Month();
                int day = date.Day();
                int hour = date.Hour();
                int minute = date.Minute();
                double second = date.Second();
                jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                jobject tsidate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                double tsivalue = bs->GetDat(date).Value();
                jobject tsi = env->NewObject(tsiclass, tsiinit, tsidate, tsivalue);
                env->SetObjectArrayElement(tsis, i - index, tsi);
            }
            return tsis;
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        jobjectArray es = env->NewObjectArray(1, eclass, NULL);
        env->SetObjectArrayElement(es, 0, e);
        return es;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetFirstDateSerie
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetFirstDate(JNIEnv *env, jobject, jstring varname) {
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        BDate date = bs->FirstDate();
        int year = date.Year();
        int month = date.Month();
        int day = date.Day();
        int hour = date.Hour();
        int minute = date.Minute();
        double second = date.Second();
        jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
        jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
        jobject tdate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
        return tdate;
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetLastDateSerie
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSerieGetLastDate(JNIEnv *env, jobject, jstring varname) {
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSerie()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSerie *bs = Tsr(bso);
        BDate date = bs->LastDate();
        int year = date.Year();
        int month = date.Month();
        int day = date.Day();
        int hour = date.Hour();
        int minute = date.Minute();
        double second = date.Second();
        jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
        jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
        jobject tdate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
        return tdate;
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeGetTimeSetDates
 * Signature: (Ljava/lang/String;Lcom/hcarralero/toljava/TOLDate;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeTimeSetGetDates(JNIEnv *env, jobject, jstring varname, jobject date, jint size)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraTimeSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSet *bs = Tms(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            jobjectArray es = env->NewObjectArray(1, eclass, NULL);
            env->SetObjectArrayElement(es, 0, e);
            return es;
        } else {
            jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
            jmethodID mgetYear = env->GetMethodID(tdclass, "getYear", "()I");
            int year = env->CallIntMethod(date, mgetYear);
            jmethodID mgetMonth = env->GetMethodID(tdclass, "getMonth", "()I");
            int month = env->CallIntMethod(date, mgetMonth);
            jmethodID mgetDay = env->GetMethodID(tdclass, "getDay", "()I");
            int day = env->CallIntMethod(date, mgetDay);
            jmethodID mgetHour = env->GetMethodID(tdclass, "getHour", "()I");
            int hour = env->CallIntMethod(date, mgetHour);
            jmethodID mgetMinute = env->GetMethodID(tdclass, "getMinute", "()I");
            int minute = env->CallIntMethod(date, mgetMinute);
            jmethodID mgetSecond = env->GetMethodID(tdclass, "getSecond", "()D");
            double second = env->CallDoubleMethod(date, mgetSecond);
            BDate date;
            date.PutDate(year, month, day, hour, minute, second);
            if (!bs->Contain(date)) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Fecha NO VÁLIDA para el TimeSet");
                jobject e = env->NewObject(eclass, einit, 6, msg);
                jobjectArray es = env->NewObjectArray(1, eclass, NULL);
                env->SetObjectArrayElement(es, 0, e);
                return es;
            } else {
                jobjectArray tds = env->NewObjectArray(size, tdclass, NULL);
                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                int i= 0;
                for (; i<size; ++i) {
                    int year = date.Year();
                    int month = date.Month();
                    int day = date.Day();
                    int hour = date.Hour();
                    int minute = date.Minute();
                    double second = date.Second();
                    jobject tdate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                    env->SetObjectArrayElement(tds, i, tdate);
                    date= bs->Successor(date);
                    if (date==BDate::End()) {
                        break;
                    }
                }
                if (i<size) {
                    size= i;
                    jobjectArray tdsa = env->NewObjectArray(size, tdclass, NULL);
                    for (i= 0; i<size; ++i) {
                        jobject tdate= env->GetObjectArrayElement(tds, i);
                        env->SetObjectArrayElement(tdsa, i, tdate);
                    }
                    env->DeleteLocalRef(tds);
                    return tdsa;
                } else {
                    return tds;
                }
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        jobjectArray es = env->NewObjectArray(1, eclass, NULL);
        env->SetObjectArrayElement(es, 0, e);
        return es;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeTimeSetGetPrevDate
 * Signature: (Ljava/lang/String;Lcom/hcarralero/toljava/TOLDate;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeTimeSetGetPrevDate(JNIEnv *env, jobject, jstring varname, jobject date)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraTimeSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSet *bs = Tms(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
            jmethodID mgetYear = env->GetMethodID(tdclass, "getYear", "()I");
            int year = env->CallIntMethod(date, mgetYear);
            jmethodID mgetMonth = env->GetMethodID(tdclass, "getMonth", "()I");
            int month = env->CallIntMethod(date, mgetMonth);
            jmethodID mgetDay = env->GetMethodID(tdclass, "getDay", "()I");
            int day = env->CallIntMethod(date, mgetDay);
            jmethodID mgetHour = env->GetMethodID(tdclass, "getHour", "()I");
            int hour = env->CallIntMethod(date, mgetHour);
            jmethodID mgetMinute = env->GetMethodID(tdclass, "getMinute", "()I");
            int minute = env->CallIntMethod(date, mgetMinute);
            jmethodID mgetSecond = env->GetMethodID(tdclass, "getSecond", "()D");
            double second = env->CallDoubleMethod(date, mgetSecond);
            BDate date;
            date.PutDate(year, month, day, hour, minute, second);
            BDate pdate= bs->Predecessor(date);
            if (pdate==BDate::Begin()) {
                jfieldID fidBD= env->GetStaticFieldID(tdclass, "BEGIN_DATE", "Lcom/hcarralero/TOLDate;");
                jobject tdate= env->GetStaticObjectField(tdclass, fidBD);
                return tdate;
            } else {
                int year = pdate.Year();
                int month = pdate.Month();
                int day = pdate.Day();
                int hour = pdate.Hour();
                int minute = pdate.Minute();
                double second = pdate.Second();
                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                jobject tdate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                return tdate;
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeTimeSetGetNextDate
 * Signature: (Ljava/lang/String;Lcom/hcarralero/toljava/TOLDate;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeTimeSetGetNextDate(JNIEnv *env, jobject, jstring varname, jobject date)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraTimeSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserTimeSet *bs = Tms(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
            jmethodID mgetYear = env->GetMethodID(tdclass, "getYear", "()I");
            int year = env->CallIntMethod(date, mgetYear);
            jmethodID mgetMonth = env->GetMethodID(tdclass, "getMonth", "()I");
            int month = env->CallIntMethod(date, mgetMonth);
            jmethodID mgetDay = env->GetMethodID(tdclass, "getDay", "()I");
            int day = env->CallIntMethod(date, mgetDay);
            jmethodID mgetHour = env->GetMethodID(tdclass, "getHour", "()I");
            int hour = env->CallIntMethod(date, mgetHour);
            jmethodID mgetMinute = env->GetMethodID(tdclass, "getMinute", "()I");
            int minute = env->CallIntMethod(date, mgetMinute);
            jmethodID mgetSecond = env->GetMethodID(tdclass, "getSecond", "()D");
            double second = env->CallDoubleMethod(date, mgetSecond);
            BDate date;
            date.PutDate(year, month, day, hour, minute, second);
            BDate pdate= bs->Successor(date);
            if (pdate==BDate::End()) {
                jfieldID fidBD= env->GetStaticFieldID(tdclass, "END_DATE", "Lcom/hcarralero/TOLDate;");
                jobject tdate= env->GetStaticObjectField(tdclass, fidBD);
                return tdate;
            } else {
                int year = pdate.Year();
                int month = pdate.Month();
                int day = pdate.Day();
                int hour = pdate.Hour();
                int minute = pdate.Minute();
                double second = pdate.Second();
                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                jobject tdate = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                return tdate;
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeSetGetSize
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSetGetSize(JNIEnv *env, jobject, jstring varname)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserSet *bs = USet(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            int size= bs->Contens().Card();
            jclass iclass = env->FindClass("Ljava/lang/Integer;");
            jmethodID iinit = env->GetMethodID(iclass, "<init>", "(I)V");
            jobject o= env->NewObject(iclass, iinit, size);
            return o;
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeSetGetElement
 * Signature: (Ljava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSetGetElement(JNIEnv *env, jobject tm, jstring varname, jint index)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserSet *bs = USet(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            return e;
        } else {
            int size= bs->Contens().Card();
            if (index<1||index>size) {
                jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                jstring msg = env->NewStringUTF("Índice fuera de rango");
                jobject e = env->NewObject(eclass, einit, 2, msg);
                return e;
            } else {
                BSyntaxObject *bse= bs->Contens().GetElement(index);
                if (bse==NULL) {
                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                    jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
                    jobject e = env->NewObject(eclass, einit, 2, msg);
                    return e;
                } else {
                    jobject iset= NULL;
                    BUserText *btxt = UText(bse);
                    if (btxt != NULL) {
                        jclass ttclass = env->FindClass("Lcom/hcarralero/toljava/TOLText;");
                        jmethodID ttinit = env->GetMethodID(ttclass, "<init>", "(Ljava/lang/String;)V");
                        jstring ttvalue = env->NewStringUTF(btxt->Contens().String());
                        iset = env->NewObject(ttclass, ttinit, ttvalue);
                    } else  {
                        BUserDat *br = UDat(bse);
                        if (br != NULL) {
                            jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLReal;");
                            jmethodID trinit = env->GetMethodID(trclass, "<init>", "(D)V");
                            double trvalue = br->Contens().GetValue();
                            iset = env->NewObject(trclass, trinit, trvalue);
                        } else {
                            BUserDate *bd = UDate(bse);
                            if (bd != NULL) {
                                jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                                BDate date = bd->Contens();
                                int year = date.Year();
                                int month = date.Month();
                                int day = date.Day();
                                int hour = date.Hour();
                                int minute = date.Minute();
                                double second = date.Second();
                                iset = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                            } else {
                                BUserCmp *bc = UCmp(bse);
                                if (bc != NULL) {
                                    jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLComplex;");
                                    jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(DD)V");
                                    double x = bc->Contens().X().GetValue();
                                    double y = bc->Contens().Y().GetValue();
                                    iset = env->NewObject(tcclass, tcinit, x, y);
                                } else {
                                    BUserMat *bmat = UMat(bse);
                                    if (bmat != NULL) {
                                        jclass tmclass = env->FindClass("Lcom/hcarralero/toljava/TOLMatrix;");
                                        jmethodID tminit = NULL;
                                        BMatrix<BDat> mat = bmat->Contens();
                                        int rows = mat.Rows();
                                        int columns = mat.Columns();
                                        if (rows <= 0 || columns <= 0) {
                                            tminit = env->GetMethodID(tmclass, "<init>", "()V");
                                            iset = env->NewObject(tmclass, tminit);
                                        } else {
                                            tminit = env->GetMethodID(tmclass, "<init>", "([[D)V");
                                            BArray<BDat> vals = mat.Data();
                                            jclass adclass = env->FindClass("[D");
                                            jobjectArray tmvalues = env->NewObjectArray(rows, adclass, NULL);
                                            for (int r = 0; r < rows; ++r) {
                                                jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                                jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                                for (int c = 0; c < columns; ++c) {
                                                    valuesRow[c] = vals[r * columns + c].Value();
                                                }
                                                env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                                env->SetObjectArrayElement(tmvalues, r, arrayRow);
                                            }
                                            iset = env->NewObject(tmclass, tminit, tmvalues);
                                        }
                                    } else {
                                        BUserVMat *bvmat = UVMat(bse);
                                        if (bvmat != NULL) {
                                            jclass tvmclass = env->FindClass("Lcom/hcarralero/toljava/TOLVMatrix;");
                                            jmethodID tvminit = NULL;
                                            BVMat vmat = bvmat->Contens();
                                            int rows = vmat.Rows();
                                            int columns = vmat.Columns();
                                            if (rows <= 0 || columns <= 0) {
                                                tvminit = env->GetMethodID(tvmclass, "<init>", "()V");
                                                iset = env->NewObject(tvmclass, tvminit);
                                            } else {
                                                tvminit = env->GetMethodID(tvmclass, "<init>", "([[D)V");
                                                BMatrix<double> mat = vmat.GetDMat();
                                                BArray<double> vals = mat.Data();
                                                jclass adclass = env->FindClass("[D");
                                                jobjectArray tvmvalues = env->NewObjectArray(rows, adclass, NULL);
                                                for (int r = 0; r < rows; ++r) {
                                                    jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                                    jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                                    for (int c = 0; c < columns; ++c) {
                                                        valuesRow[c] = vals[r * columns + c];
                                                    }
                                                    env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                                    env->SetObjectArrayElement(tvmvalues, r, arrayRow);
                                                }
                                                iset = env->NewObject(tvmclass, tvminit, tvmvalues);
                                            }
                                        } else {
                                            BUserPol *bp = UPol(bse);
                                            if (bp != NULL) {
                                                jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                                jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                                BPolyn<BDat> pol = bp->Contens();
                                                int nt = pol.Size();
                                                jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                                iset = env->NewObject(tpclass, tpinit);
                                                for (int i = 0; i < nt; ++i) {
                                                    BMonome<BDat> bm = pol[i];
                                                    int var = 0;
                                                    int grade = bm.Degree();
                                                    if (grade < 0) {
                                                        grade = -grade;
                                                        var = 1;
                                                    }
                                                    double coef = bm.Coef().GetValue();
                                                    env->CallVoidMethod(iset, maddTerm, var, coef, grade);
                                                }
                                            } else {
                                                BUserRat *br = URat(bse);
                                                if (br != NULL) {
                                                    BRational<BDat> rat = br->Contens();
                                                    jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLRatio;");
                                                    jmethodID trinit = env->GetMethodID(trclass, "<init>", "(Lcom/hcarralero/toljava/TOLPolyn;Lcom/hcarralero/toljava/TOLPolyn;)V");
                                                    jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                                    jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                                    BPolyn<BDat> pnum = rat.Numerator();
                                                    int nt = pnum.Size();
                                                    jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                                    jobject valueNum = env->NewObject(tpclass, tpinit);
                                                    for (int i = 0; i < nt; ++i) {
                                                        BMonome<BDat> bmnum = pnum[i];
                                                        int var = 0;
                                                        int grade = bmnum.Degree();
                                                        if (grade < 0) {
                                                            grade = -grade;
                                                            var = 1;
                                                        }
                                                        double coef = bmnum.Coef().GetValue();
                                                        env->CallVoidMethod(valueNum, maddTerm, var, coef, grade);
                                                    }
                                                    BPolyn<BDat> pden = rat.Denominator();
                                                    nt = pden.Size();
                                                    jobject valueDen = env->NewObject(tpclass, tpinit);
                                                    for (int i = 0; i < nt; ++i) {
                                                        BMonome<BDat> bmden = pden[i];
                                                        int var = 0;
                                                        int grade = bmden.Degree();
                                                        if (grade < 0) {
                                                            grade = -grade;
                                                            var = 1;
                                                        }
                                                        double coef = bmden.Coef().GetValue();
                                                        env->CallVoidMethod(valueDen, maddTerm, var, coef, grade);
                                                    }
                                                    iset = env->NewObject(trclass, trinit, valueNum, valueDen);
                                                } else {
                                                    BUserCode *bc = UCode(bse);
                                                    if (bc != NULL) {
                                                        jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
                                                        jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
                                                        BCode cod = bc->Contens();
                                                        jstring name = env->NewStringUTF(cod.Name().String());
                                                        jstring info = env->NewStringUTF(cod.Info().String());
                                                        iset = env->NewObject(tcclass, tcinit, name, info);
                                                    } else {
                                                        BUserTimeSerie *bs = Tsr(bse);
                                                        if (bs != NULL) {
                                                            jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSerie;");
                                                            jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                            char *cvname= (char *)malloc(strlen(ccname)+6);
                                                            if (cvname!=NULL) {
                                                                sprintf(cvname, "%s[%d]", ccname, index);
                                                                jstring vn = env->NewStringUTF(cvname);
                                                                iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                            }
                                                        } else {
                                                            BUserTimeSet *bt = Tms(bse);
                                                            if (bt != NULL) {
                                                                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRTimeSet;");
                                                                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                                char *cvname= (char *)malloc(strlen(ccname)+6);
                                                                if (cvname!=NULL) {
                                                                    sprintf(cvname, "%s[%d]", ccname, index);
                                                                    jstring vn = env->NewStringUTF(cvname);
                                                                    iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                                }
                                                            } else {
                                                                BUserSet *bs = USet(bso);
                                                                if (bs != NULL) {
                                                                    jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSet;");
                                                                    jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                                    char *cvname= (char *)malloc(strlen(ccname)+6);
                                                                    if (cvname!=NULL) {
                                                                        sprintf(cvname, "%s[%d]", ccname, index);
                                                                        jstring vn = env->NewStringUTF(cvname);
                                                                        iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (iset == NULL) {
                        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                        jstring msg = env->NewStringUTF("Error DESCONOCIDO");
                        jobject e = env->NewObject(eclass, einit, 4, msg);
                        return e;
                    } else {
                        return iset;
                    }
                }
            }
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        return e;
    }
}

/*
 * Class:     com_hcarralero_toljava_TOLMachine
 * Method:    nativeSetGetElements
 * Signature: (Ljava/lang/String;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_hcarralero_toljava_TOLMachine_nativeSetGetElements(JNIEnv *env, jobject tm, jstring varname)
{
    borrarMensajes();
    const char *ccname = env->GetStringUTFChars(varname, NULL);
    BSyntaxObject *bso = GraSet()->EvaluateExpr(ccname);
    if (bso != NULL) {
        BUserSet *bs = USet(bso);
        if (bs == NULL) {
            jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
            jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
            jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
            jobject e = env->NewObject(eclass, einit, 2, msg);
            jobjectArray es = env->NewObjectArray(1, eclass, NULL);
            env->SetObjectArrayElement(es, 0, e);
            return es;
        } else {
            int size= bs->Contens().Card();
            jclass toclass = env->FindClass("Lcom/hcarralero/toljava/TOLObject;");
            jobjectArray tovalues = env->NewObjectArray(size, toclass, NULL);
            for (int i= 0; i<size; ++i) {
                BSyntaxObject *bse= bs->Contens().GetElement(i+1);
                if (bse==NULL) {
                    jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
                    jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
                    jstring msg = env->NewStringUTF("ERROR DESCONOCIDO");
                    jobject e = env->NewObject(eclass, einit, 2, msg);
                    env->SetObjectArrayElement(tovalues, 0, e);
                    return tovalues;
                } else {
                    jobject iset= NULL;
                    BUserText *btxt = UText(bse);
                    if (btxt != NULL) {
                        jclass ttclass = env->FindClass("Lcom/hcarralero/toljava/TOLText;");
                        jmethodID ttinit = env->GetMethodID(ttclass, "<init>", "(Ljava/lang/String;)V");
                        jstring ttvalue = env->NewStringUTF(btxt->Contens().String());
                        iset = env->NewObject(ttclass, ttinit, ttvalue);
                    } else  {
                        BUserDat *br = UDat(bse);
                        if (br != NULL) {
                            jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLReal;");
                            jmethodID trinit = env->GetMethodID(trclass, "<init>", "(D)V");
                            double trvalue = br->Contens().GetValue();
                            iset = env->NewObject(trclass, trinit, trvalue);
                        } else {
                            BUserDate *bd = UDate(bse);
                            if (bd != NULL) {
                                jclass tdclass = env->FindClass("Lcom/hcarralero/toljava/TOLDate;");
                                jmethodID tdinit = env->GetMethodID(tdclass, "<init>", "(IIIIID)V");
                                BDate date = bd->Contens();
                                int year = date.Year();
                                int month = date.Month();
                                int day = date.Day();
                                int hour = date.Hour();
                                int minute = date.Minute();
                                double second = date.Second();
                                iset = env->NewObject(tdclass, tdinit, year, month, day, hour, minute, second);
                            } else {
                                BUserCmp *bc = UCmp(bse);
                                if (bc != NULL) {
                                    jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLComplex;");
                                    jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(DD)V");
                                    double x = bc->Contens().X().GetValue();
                                    double y = bc->Contens().Y().GetValue();
                                    iset = env->NewObject(tcclass, tcinit, x, y);
                                } else {
                                    BUserMat *bmat = UMat(bse);
                                    if (bmat != NULL) {
                                        jclass tmclass = env->FindClass("Lcom/hcarralero/toljava/TOLMatrix;");
                                        jmethodID tminit = NULL;
                                        BMatrix<BDat> mat = bmat->Contens();
                                        int rows = mat.Rows();
                                        int columns = mat.Columns();
                                        if (rows <= 0 || columns <= 0) {
                                            tminit = env->GetMethodID(tmclass, "<init>", "()V");
                                            iset = env->NewObject(tmclass, tminit);
                                        } else {
                                            tminit = env->GetMethodID(tmclass, "<init>", "([[D)V");
                                            BArray<BDat> vals = mat.Data();
                                            jclass adclass = env->FindClass("[D");
                                            jobjectArray tmvalues = env->NewObjectArray(rows, adclass, NULL);
                                            for (int r = 0; r < rows; ++r) {
                                                jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                                jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                                for (int c = 0; c < columns; ++c) {
                                                    valuesRow[c] = vals[r * columns + c].Value();
                                                }
                                                env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                                env->SetObjectArrayElement(tmvalues, r, arrayRow);
                                            }
                                            iset = env->NewObject(tmclass, tminit, tmvalues);
                                        }
                                    } else {
                                        BUserVMat *bvmat = UVMat(bse);
                                        if (bvmat != NULL) {
                                            jclass tvmclass = env->FindClass("Lcom/hcarralero/toljava/TOLVMatrix;");
                                            jmethodID tvminit = NULL;
                                            BVMat vmat = bvmat->Contens();
                                            int rows = vmat.Rows();
                                            int columns = vmat.Columns();
                                            if (rows <= 0 || columns <= 0) {
                                                tvminit = env->GetMethodID(tvmclass, "<init>", "()V");
                                                iset = env->NewObject(tvmclass, tvminit);
                                            } else {
                                                tvminit = env->GetMethodID(tvmclass, "<init>", "([[D)V");
                                                BMatrix<double> mat = vmat.GetDMat();
                                                BArray<double> vals = mat.Data();
                                                jclass adclass = env->FindClass("[D");
                                                jobjectArray tvmvalues = env->NewObjectArray(rows, adclass, NULL);
                                                for (int r = 0; r < rows; ++r) {
                                                    jdoubleArray arrayRow = env->NewDoubleArray(columns);
                                                    jdouble *valuesRow = env->GetDoubleArrayElements(arrayRow, NULL);
                                                    for (int c = 0; c < columns; ++c) {
                                                        valuesRow[c] = vals[r * columns + c];
                                                    }
                                                    env->SetDoubleArrayRegion(arrayRow, 0, columns, valuesRow);
                                                    env->SetObjectArrayElement(tvmvalues, r, arrayRow);
                                                }
                                                iset = env->NewObject(tvmclass, tvminit, tvmvalues);
                                            }
                                        } else {
                                            BUserPol *bp = UPol(bse);
                                            if (bp != NULL) {
                                                jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                                jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                                BPolyn<BDat> pol = bp->Contens();
                                                int nt = pol.Size();
                                                jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                                iset = env->NewObject(tpclass, tpinit);
                                                for (int j = 0; j < nt; ++j) {
                                                    BMonome<BDat> bm = pol[j];
                                                    int var = 0;
                                                    int grade = bm.Degree();
                                                    if (grade < 0) {
                                                        grade = -grade;
                                                        var = 1;
                                                    }
                                                    double coef = bm.Coef().GetValue();
                                                    env->CallVoidMethod(iset, maddTerm, var, coef, grade);
                                                }
                                            } else {
                                                BUserRat *br = URat(bse);
                                                if (br != NULL) {
                                                    BRational<BDat> rat = br->Contens();
                                                    jclass trclass = env->FindClass("Lcom/hcarralero/toljava/TOLRatio;");
                                                    jmethodID trinit = env->GetMethodID(trclass, "<init>", "(Lcom/hcarralero/toljava/TOLPolyn;Lcom/hcarralero/toljava/TOLPolyn;)V");
                                                    jclass tpclass = env->FindClass("Lcom/hcarralero/toljava/TOLPolyn;");
                                                    jmethodID tpinit = env->GetMethodID(tpclass, "<init>", "()V");
                                                    BPolyn<BDat> pnum = rat.Numerator();
                                                    int nt = pnum.Size();
                                                    jmethodID maddTerm = env->GetMethodID(tpclass, "addTerm", "(IDI)V");
                                                    jobject valueNum = env->NewObject(tpclass, tpinit);
                                                    for (int j = 0; j < nt; ++j) {
                                                        BMonome<BDat> bmnum = pnum[j];
                                                        int var = 0;
                                                        int grade = bmnum.Degree();
                                                        if (grade < 0) {
                                                            grade = -grade;
                                                            var = 1;
                                                        }
                                                        double coef = bmnum.Coef().GetValue();
                                                        env->CallVoidMethod(valueNum, maddTerm, var, coef, grade);
                                                    }
                                                    BPolyn<BDat> pden = rat.Denominator();
                                                    nt = pden.Size();
                                                    jobject valueDen = env->NewObject(tpclass, tpinit);
                                                    for (int j = 0; j < nt; ++j) {
                                                        BMonome<BDat> bmden = pden[j];
                                                        int var = 0;
                                                        int grade = bmden.Degree();
                                                        if (grade < 0) {
                                                            grade = -grade;
                                                            var = 1;
                                                        }
                                                        double coef = bmden.Coef().GetValue();
                                                        env->CallVoidMethod(valueDen, maddTerm, var, coef, grade);
                                                    }
                                                    iset = env->NewObject(trclass, trinit, valueNum, valueDen);
                                                } else {
                                                    BUserCode *bc = UCode(bse);
                                                    if (bc != NULL) {
                                                        jclass tcclass = env->FindClass("Lcom/hcarralero/toljava/TOLCode;");
                                                        jmethodID tcinit = env->GetMethodID(tcclass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
                                                        BCode cod = bc->Contens();
                                                        jstring name = env->NewStringUTF(cod.Name().String());
                                                        jstring info = env->NewStringUTF(cod.Info().String());
                                                        iset = env->NewObject(tcclass, tcinit, name, info);
                                                    } else {
                                                        BUserTimeSerie *bs = Tsr(bse);
                                                        if (bs != NULL) {
                                                            jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSerie;");
                                                            jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                            char *cvname= (char *)malloc(strlen(ccname)+6);
                                                            if (cvname!=NULL) {
                                                                sprintf(cvname, "%s[%d]", ccname, i+1);
                                                                jstring vn = env->NewStringUTF(cvname);
                                                                iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                            }
                                                        } else {
                                                            BUserTimeSet *bt = Tms(bse);
                                                            if (bt != NULL) {
                                                                jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRTimeSet;");
                                                                jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                                char *cvname= (char *)malloc(strlen(ccname)+6);
                                                                if (cvname!=NULL) {
                                                                    sprintf(cvname, "%s[%d]", ccname, i+1);
                                                                    jstring vn = env->NewStringUTF(cvname);
                                                                    iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                                }
                                                            } else {
                                                                BUserSet *bs = USet(bse);
                                                                if (bs != NULL) {
                                                                    jclass tsclass = env->FindClass("Lcom/hcarralero/toljava/TOLRSet;");
                                                                    jmethodID tsinit = env->GetMethodID(tsclass, "<init>", "(Lcom/hcarralero/toljava/TOLMachine;Ljava/lang/String;)V");
                                                                    char *cvname= (char *)malloc(strlen(ccname)+6);
                                                                    if (cvname!=NULL) {
                                                                        sprintf(cvname, "%s[%d]", ccname, i+1);
                                                                        jstring vn = env->NewStringUTF(cvname);
                                                                        iset = env->NewObject(tsclass, tsinit, tm, vn);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        env->SetObjectArrayElement(tovalues, i, iset);
                    }
                }
            }
            return tovalues;
        }
    } else {
        jclass eclass = env->FindClass("Lcom/hcarralero/toljava/TOLMachineException;");
        jmethodID einit = env->GetMethodID(eclass, "<init>", "(ILjava/lang/String;)V");
        jstring msg = env->NewStringUTF("Variable NO ENCONTRADA");
        jobject e = env->NewObject(eclass, einit, 2, msg);
        jobjectArray es = env->NewObjectArray(1, eclass, NULL);
        env->SetObjectArrayElement(es, 0, e);
        return es;
    }
}
