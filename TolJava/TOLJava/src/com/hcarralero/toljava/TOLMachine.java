package com.hcarralero.toljava;

/**
 * Representa a una instancia de la Máquina Virtual de TOL.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMachineException
 */
public class TOLMachine {

    //Inicializa una instancia de TOL
    private native String nativeInitTOL();

    //Ejecuta código TOL
    private native Object[] nativeExecute(String code);

    //Ejecuta código TOL
    private native Object nativeCallCode(String codeName, Object[] parameters);

    //Obtiene una variable del entorno de ejecución de TOL
    private native Object nativeGetVariable(String varName);

    //Cambia el valor a una variable del entorno ejecución de TOL
    private native Object nativeSetVariableValue(String varName, TOLLObject varValue);

    //Obtiene un arreglo con todas las viariables tipo Text definidas en TOL
    private native TOLVariable[] nativeGetTextVariables();

    //Obtiene un arreglo con todas las viariables tipo Real definidas en TOL
    private native TOLVariable[] nativeGetRealVariables();

    //Obtiene un arreglo con todas las viariables tipo Date definidas en TOL
    private native TOLVariable[] nativeGetDateVariables();

    //Obtiene un arreglo con todas las viariables tipo Complex definidas en TOL
    private native TOLVariable[] nativeGetComplexVariables();

    //Obtiene un arreglo con todas las viariables tipo Matrix definidas en TOL
    private native TOLVariable[] nativeGetMatrixVariables();

    //Obtiene un arreglo con todas las viariables tipo VMatrix definidas en TOL
    private native TOLVariable[] nativeGetVMatrixVariables();

    //Obtiene un arreglo con todas las viariables tipo Polyn definidas en TOL
    private native TOLVariable[] nativeGetPolynVariables();

    //Obtiene un arreglo con todas las viariables tipo Polyn definidas en TOL
    private native TOLVariable[] nativeGetRatioVariables();

    //Obtiene un arreglo con todas las viariables tipo Code definidas en TOL
    private native TOLVariable[] nativeGetCodeVariables();

    //Obtiene un arreglo con todas las viariables tipo Serie definidas en TOL
    private native TOLVariable[] nativeGetSerieVariables();

    //Obtiene un arreglo con todas las viariables tipo TimeSet definidas en TOL
    private native TOLVariable[] nativeGetTimeSetVariables();

    //Obtiene un arreglo con todas las viariables tipo Set definidas en TOL
    private native TOLVariable[] nativeGetSetVariables();

    //Obtiene un arreglo con todos los NameBlocks definidos en TOL
    private native TOLNameBlock[] nativeGetNameBlocks();

    //Obtiene el valor de una serie en una fecha dada
    private native Object nativeSerieGetValue(String varname, TOLDate date);

    //Obtiene el valor de una serie en un índice dado
    private native Object nativeSerieGetValue(String varname, int index);

    //Obtiene un arreglo de Items de una serie a partir de una fecha dada
    private native Object[] nativeSerieGetItems(String varname, TOLDate date, int size);

    //Obtiene un arreglo de Items de una serie a partir de una posición dada
    private native Object[] nativeSerieGetItems(String varname, int index, int size);

    //Obtiene la primera fecha de una serie
    private native Object nativeSerieGetFirstDate(String varname);

    //Obtiene la última fecha de una serie
    private native Object nativeSerieGetLastDate(String varname);

    //Obtiene un arreglo de fechas de un TimeSet a partir de una fecha dada
    private native Object[] nativeTimeSetGetDates(String varname, TOLDate date, int size);

    //Obtiene la fecha previa a una dada en un TimeSet
    private native Object nativeTimeSetGetPrevDate(String varname, TOLDate date);

    //Obtiene la fecha siguiente a una dada en un TimeSet
    private native Object nativeTimeSetGetNextDate(String varname, TOLDate date);

    //Obtiene el tamaño de un Set
    private native Object nativeSetGetSize(String varname);

    //Obtiene el elemento de la posición index de un conjunto
    private native Object nativeSetGetElement(String varname, int index);

    //Obtiene todos los elementos de un conjunto como un arreglo
    private native Object[] nativeSetGetElements(String varname);

    /**
     * Crea una instancia de la Máquina Virtual de TOL y la inicializa
     *
     */
    public TOLMachine() {
        nativeInitTOL();
    }

    /**
     * Inicializa la Máquina Virtual de TOL.
     * 
     */
    public void initTOL() {
        nativeInitTOL();
    }

    /**
     * Ejecuta en la Máquina Virtual de TOL el código pasado por parámetro
     * 
     * @param code código TOL a ejecutarse
     * @return un arreglo de objetos cuyo primer elemento es el número de errores
     * que se han producido tanto en la compilación como en la ejecución, el resto
     * de los elementos son los mensajes generados por la Máquina Virtual de TOL,
     * lo cual es variable. (Consulte la documentación de TOL para revisar el
     * formato de los mensajes producidos y poder procesarlos)
     */
    public Object[] execute(String code) {
        return nativeExecute(code);
    }

    /**
     * Hace una llamada a una función asignada a una variable de tipo Code
     *
     * @param codeName nombre de la variable tipo Code a la que se hace la llamada
     * @param parameters arreglo contiendo los parámetros que se deben pasar a la
     * función que se llama; cada elemento de este arreglo puede ser un objeto
     * perteneciente a una clase derivada de <code>TOLLObject</code> o una cadena con el
     * nombre de una variable definida en TOL
     * @return un objeto con el valor retornado por la función que se ha invocado
     * @throws com.hcarralero.toljava.TOLMachineException cuando algún parámetro no es
     * correcto, el nombre de la función a invocar no se ha encontrado u ocurre alguna
     * otra anomalía en la evaluación
     */
    public TOLObject callCode(String codeName, Object[] parameters) throws TOLMachineException {
//        String strParameters[] = new String[parameters.length];
//        for (int i = 0; i < parameters.length; ++i) {
//            if (parameters[i].getClass() == TOLText.class) {
//                TOLText tt = (TOLText) parameters[i];
//                strParameters[i] = "\"" + tt.getText() + "\"";
//            } else {
//                if (parameters[i].getClass() == TOLReal.class) {
//                    TOLReal tr = (TOLReal) parameters[i];
//                    strParameters[i] = "" + tr.getValue();
//                } else {
//                    if (parameters[i].getClass() == TOLDate.class) {
//                        TOLDate td = (TOLDate) parameters[i];
//                        strParameters[i] = "y" + td.getYear() + "m" + td.getMonth() + "d" + 
//                                              td.getDay() + "h" + td.getHour() + "i" +
//                                                td.getMinute() + "s" + td.getSecond();
//                    } else {
//                        if (parameters[i].getClass() == TOLComplex.class) {
//                            TOLComplex tc = (TOLComplex) parameters[i];
//                            double x = tc.getX();
//                            double y = tc.getY();
//                            if (y != 0) {
//                                strParameters[i] = "" + tc.getY() + "*i";
//                            }
//                            if (x > 0) {
//                                strParameters[i] += "+" + tc.getX();
//                            }
//                            if (x < 0) {
//                                strParameters[i] += tc.getX();
//                            }
//                            if (x == 0 && y == 0) {
//                                strParameters[i] += "0";
//                            }
//                        } else {
//                            if (parameters[i].getClass() == TOLMatrix.class) {
//                                TOLMatrix tm = (TOLMatrix) parameters[i];
//                                int rows= tm.getRows();
//                                int columns= tm.getColumns();
//                                if (rows==0||columns==0) {
//                                    strParameters[i] = "UnknownMatrix";
//                                } else {
//                                    strParameters[i] = "(";
//                                    for (int m= 0; m<rows; ++m) {
//                                        strParameters[i] += "(";
//                                        boolean coma= false;
//                                        for (int n= 0; n<columns; ++n) {
//                                            if (!coma) {
//                                                coma= true;
//                                            } else {
//                                                strParameters[i] += ",";
//                                            }
//                                            try {
//                                                strParameters[i] += ""+tm.getCellValue(m, n);
//                                            } catch (TOLMatrixException ex) {
//                                            }
//                                        }
//                                        strParameters[i] += ")";
//                                    }
//                                    strParameters[i] += ")";
//                                }
//                            } else {
//                                if (parameters[i].getClass() == TOLPolyn.class) {
//                                    TOLPolyn tp = (TOLPolyn) parameters[i];
//                                    strParameters[i]= tp.toString();
//                                } else {
//                                    if (parameters[i].getClass() == TOLRatio.class) {
//                                        TOLPolyn tp = (TOLPolyn) parameters[i];
//                                        strParameters[i]= tp.toString();
//                                    } else {
//                                        strParameters[i]= parameters[i].toString();
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            strParameters[i] = parameters[i].toString();
//        }
//        Object res= nativeCallCode(codeName, strParameters);
        Object res= nativeCallCode(codeName, parameters);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        }
        return (TOLObject)res;
    }

    /**
     * Determina el valor de una variable dado su nombre como parámetro
     *
     * @param varName nombre de la variable en la Máquina Virtual de TOL
     * @return una variable instanciada con los datos obtenidos de TOL
     * @throws TOLMachineException cuando la variable NO SE ENCONTRÓ
     */
    public TOLVariable getVariable(String varName) throws TOLMachineException {
        Object o = nativeGetVariable(varName);
        if (o.getClass() == TOLMachineException.class) {
            TOLMachineException e = (TOLMachineException) o;
            throw e;
        }
        TOLVariable var = (TOLVariable) o;
        return var;
    }

    /**
     * Cambia el valor de la variable cuyo nombre se indica por el valor pasado
     * como parámetro
     *
     * @param varName nombre de la variable
     * @param varValue nuevo valor de la variable
     * @throws TOLMachineException cuando no se encuentra la variable o se intenta
     * asignar un valor de un tipo erróneo
     */
    public void setVariableValue(String varName, TOLLObject varValue) throws TOLMachineException {
        Object o = nativeSetVariableValue(varName, varValue);
        if (o != null && o.getClass() == TOLMachineException.class) {
            TOLMachineException e = (TOLMachineException) o;
            throw e;
        }
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Text
     *
     * @return el arreglo con todas las variables de tipo Text
     */
    public TOLVariable[] getTextVariables() {
        return nativeGetTextVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Real
     *
     * @return el arreglo con todas las variables de tipo Real
     */
    public TOLVariable[] getRealVariables() {
        return nativeGetRealVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Date
     *
     * @return el arreglo con todas las variables de tipo Date
     */
    public TOLVariable[] getDateVariables() {
        return nativeGetDateVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Complex
     *
     * @return el arreglo con todas las variables de tipo Complex
     */
    public TOLVariable[] getComplexVariables() {
        return nativeGetComplexVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Matrix
     *
     * @return el arreglo con todas las variables de tipo Matrix
     */
    public TOLVariable[] getMatrixVariables() {
        return nativeGetMatrixVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo VMatrix
     *
     * @return el arreglo con todas las variables de tipo VMatrix
     */
    public TOLVariable[] getVMatrixVariables() {
        return nativeGetVMatrixVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Polyn
     *
     * @return el arreglo con todas las variables de tipo Polyn
     */
    public TOLVariable[] getPolynVariables() {
        return nativeGetPolynVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Ratio
     *
     * @return el arreglo con todas las variables de tipo Ratio
     */
    public TOLVariable[] getRatioVariables() {
        return nativeGetRatioVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Code
     *
     * @return el arreglo con todas las variables de tipo Code
     */
    public TOLVariable[] getCodeVariables() {
        return nativeGetCodeVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Serie
     *
     * @return el arreglo con todas las variables de tipo Serie
     */
    public TOLVariable[] getSerieVariables() {
        return nativeGetSerieVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo TimeSet
     *
     * @return el arreglo con todas las variables de tipo TimeSet
     */
    public TOLVariable[] getTimeSetVariables() {
        return nativeGetTimeSetVariables();
    }

    /**
     * Obtiene un arreglo con todas las variables de tipo Set
     *
     * @return el arreglo con todas las variables de tipo Set
     */
    public TOLVariable[] getSetVariables() {
        return nativeGetSetVariables();
    }

    /**
     * Obtiene un arreglo con todas los NameBlocks definidos en TOL
     *
     * @return el arreglo con todas los NameBlocks definidos en TOL
     */
    public TOLNameBlock[] getNameBlocks() {
        return nativeGetNameBlocks();
    }

    /**
     * Obtiene el valor de una serie en una fecha dada
     *
     * @param varname nombre de la serie en TOL
     * @param date fecha en al que se quiere el valor
     * @return el valor de la serie en la fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la
     *         Serie en TOL
     */
    public double getSerieValue(String varname, TOLDate date) throws TOLMachineException {
        Object res= nativeSerieGetValue(varname, date);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        }
        return ((Double)res).doubleValue();
    }

    /**
     * Obtiene el valor de una serie en una posición dada
     *
     * @param varname nombre de la Serie en TOL
     * @param index posición del elemento en la serie
     * @return el valor de la serie en la posición dada
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la
     *         Serie en TOL
     */
    public double getSerieValue(String varname, int index) throws TOLMachineException {
        Object res= nativeSerieGetValue(varname, index);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        }
        return ((Double)res).doubleValue();
    }

    /**
     * Obtiene un arreglo de items de una serie a partir de una fecha dada
     *
     * @param varname nombre de la Serie en TOL
     * @param date fecha a partir de la cual se obtendrán los elementos
     * @param size tamaño del arreglo que se obtiene
     * @return el arreglo de items de la serie a partir de la fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la Serie en TOL
     */
    public TSItem[] getSerieItems(String varname, TOLDate date, int size) throws TOLMachineException {
        Object[] res= this.nativeSerieGetItems(varname, date, size);
        if (res==null||res.length==0) {
            return null;
        } else {
            if (res[0].getClass()==TOLMachineException.class) {
                TOLMachineException e= (TOLMachineException)res[0];
                throw e;
            } else {
                return (TSItem[])res;
            }
        }
    }

    /**
     * Obtiene un arreglo de items de una serie a partir de una posición dada
     *
     * @param varname nombre de la Serie en TOL
     * @param index posición del primer elemento en la serie
     * @param size tamaño del arreglo que se obtiene
     * @return el arreglo de items de la serie a partir de la posición dada
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la Serie en TOL
     */
    public TSItem[] getSerieItems(String varname, int index, int size) throws TOLMachineException {
        Object[] res= this.nativeSerieGetItems(varname, index, size);
        if (res==null||res.length==0) {
            return null;
        } else {
            if (res[0].getClass()==TOLMachineException.class) {
                TOLMachineException e= (TOLMachineException)res[0];
                throw e;
            } else {
                return (TSItem[])res;
            }
        }
    }

    /**
     * Obtiene la primera fecha de una serie
     *
     * @param varname nombre de la Serie en TOL
     * @return la primera fecha de la serie
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la Serie en TOL
     */
    public TOLDate getSerieFirstDate(String varname) throws TOLMachineException {
        Object res= this.nativeSerieGetFirstDate(varname);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return (TOLDate)res;
        }
    }

    /**
     * Obtiene la última fecha de una serie
     *
     * @param varname nombre de la Serie en TOL
     * @return la última fecha de la serie
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo a la Serie en TOL
     */
    public TOLDate getSerieLastDate(String varname) throws TOLMachineException {
        Object res= this.nativeSerieGetLastDate(varname);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return (TOLDate)res;
        }
    }

    /**
     * Obtiene la fecha previa a una dada en un TimeSet
     *
     * @param varname nombre del TimeSet en TOL
     * @param date fecha de la cual se obtendrá la previa
     * @return la fecha previa a la dada en el TimeSet
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al TimeSet en TOL
     */
    public TOLDate getTimeSetPrevDate(String varname, TOLDate date) throws TOLMachineException {
        Object res= this.nativeTimeSetGetPrevDate(varname, date);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return (TOLDate)res;
        }
    }

    /**
     * Obtiene la fecha siguiente a una dada en un TimeSet
     * 
     * @param varname nombre del TimeSet en TOL
     * @param date fecha de la cual se obtendrá la siguiente
     * @return la fecha siguiente a la dada en el TimeSet
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al TimeSet en TOL
     */
    public TOLDate getTimeSetNextDate(String varname, TOLDate date) throws TOLMachineException {
        Object res= this.nativeTimeSetGetNextDate(varname, date);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return (TOLDate)res;
        }
    }

    /**
     * Obtiene un arreglo de fechas de un TimeSet a partir de una fecha dada
     *
     * @param varname nombre del TimeSet en TOL
     * @param date fecha inicial en el arreglo
     * @param size tamaño del arreglo
     * @return el arreglo de fechas del TimeSet a partir de la fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al TimeSet en TOL
     */
    public TOLDate[] getTimeSetDates(String varname, TOLDate date, int size) throws TOLMachineException {
        Object[] res= this.nativeTimeSetGetDates(varname, date, size);
        if (res==null||res.length==0) {
            return null;
        } else {
            if (res[0].getClass()==TOLMachineException.class) {
                TOLMachineException e= (TOLMachineException)res[0];
                throw e;
            } else {
                return (TOLDate[])res;
            }
        }
    }

    /**
     * Obtiene el tamaño (número de elementos) de un Set
     * 
     * @param varname nombre del Set en TOL
     * @return el tamaño (número de elementos) del Set
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al Set en TOL
     */
    public int getSetSize(String varname) throws TOLMachineException {
        Object res= this.nativeSetGetSize(varname);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return ((Integer)res).intValue();
        }
    }

    /**
     * Obtiene un arreglo todos los elementos de un Set
     *
     * @param varname nombre del Set en TOL
     * @return el arreglo todos los elementos de un Set
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al Set en TOL
     */
    public TOLObject[] getSetElements(String varname) throws TOLMachineException {
        Object[] res= this.nativeSetGetElements(varname);
        if (res==null||res.length==0) {
            return null;
        } else {
            if (res[0].getClass()==TOLMachineException.class) {
                TOLMachineException e= (TOLMachineException)res[0];
                throw e;
            } else {
                return (TOLObject[])res;
            }
        }
    }

    /**
     * Obtiene el elemento de la posición index en un Set
     *
     * @param varname nombre del Set en TOL
     * @param index posición del elemento en el Set
     * @return el elemento de la posición index en el Set
     * @throws com.hcarralero.toljava.TOLMachineException si ocurre algún error accediendo al Set en TOL
     */
    public TOLObject getSetElement(String varname, int index) throws TOLMachineException {
        Object res= this.nativeSetGetElement(varname, index);
        if (res.getClass()==TOLMachineException.class) {
            TOLMachineException e= (TOLMachineException)res;
            throw e;
        } else {
            return (TOLObject)res;
        }
    }

    //Indica si ya se ha cargado en memoria la biblioteca TOLJavaJNI
    private static boolean cargadoTOLJavaJNI = false;

    //Carga en memoria la biblioteca TOLJavaJNI
    static {
        if (!cargadoTOLJavaJNI) {
            System.loadLibrary("TOLJavaJNI");
            cargadoTOLJavaJNI = true;
        }
    }

}
