package com.hcarralero.toljava;

/**
 * Representa una condición de error ocurrida durante el acceso a un objeto de
 * tipo <code>TOLMachine</code>.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLMachineException extends Exception {

    /**
     * No ha ocurrido ningún error (¡IMPOSIBLE!).
     *
     */
    public static final int NO_ERROR = 0;

    /**
     * ERROR: Fallo Interno de la Máquina Virtual de Java.
     *
     */
    public static final int  ERROR_INTERNAL_JVM_ERROR= 1;

    /**
     * ERROR: Variable NO ENCONTRADA.
     *
     */
    public static final int  ERROR_VARIABLE_NOT_FOUND= 2;

    /**
     * ERROR: Tipos de datos NO COINCIDEN.
     *
     */
    public static final int  ERROR_MISMATCH_DATATYPES= 3;

    /**
     * ERROR: Fallo Interno de TOL.
     *
     */
    public static final int  ERROR_INTERNAL_TOL_ERROR= 4;

    /**
     * ERROR: Parámetro NO VÁLIDO en llamada a Función.
     *
     */
    public static final int  ERROR_INVALID_PARAMETER= 5;

    /**
     * ERROR: Fecha NO VÁLIDA para una Serie o TimeSet.
     *
     */
    public static final int  ERROR_INVALID_DATE= 6;

    /**
     * ERROR: Índice NO VÁLIDO para una Serie, un TimeSet o un Set.
     *
     */
    public static final int  ERROR_INVALID_INDEX= 7;

    private int errorCode = NO_ERROR;

    /**
     * Retorna el código del error que provocó que se lanzara la excepción.
     *
     * @return el código del error.
     */
    public int getErrorCode() {
        return errorCode;
    }

    /**
     * Construye una instancia de <code>TOLMachineException</code> con el código y mensaje de error especificados.
     * @param errorCode Código del error que provocó que se lanzara la excepción.
     * @param msg mensaje describiendo el error que provocó que se lanzara la excepción.
     */
    public TOLMachineException(int errorCode, String msg) {
        super(msg);
        this.errorCode= errorCode;
    }
}
