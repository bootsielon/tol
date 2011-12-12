
package com.hcarralero.toljava;

/**
 * Representa una condición de error ocurrida durante la manipulación de un
 * objeto de tipo <code>TOLMatrix</code> o <code>TOLVMatrix</code>.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLMatrixException extends Exception {

    /**
     * No ha ocurrido ningún error (¡IMPOSIBLE!).
     *
     */
    public static final int NO_ERROR = 0;

    /**
     * ERROR: Número de filas de la matriz NO ES VÁLIDO (<=0).
     *
     */
    public static final int  ERROR_INVALID_ROWS= 1;

    /**
     * ERROR: Número de columnas de la matriz NO ES VÁLIDO (<=0).
     *
     */
    public static final int  ERROR_INVALID_COLUMNS= 2;

    /**
     * ERROR: Se intenta acceder a un NÚMERO DE FILA NO VÁLIDO (<0 o >número de filas de la matriz).
     *
     */
    public static final int  ERROR_INVALID_ROW_NUMBER= 3;

    /**
     * ERROR: Se intenta acceder a un NÚMERO DE COLUMNA NO VÁLIDO (<0 o >número de columnas de la matriz).
     *
     */
    public static final int  ERROR_INVALID_COLUMN_NUMBER= 4;

    /**
     * ERROR: No hay memoria suficiente para crear la matriz.
     *
     */
    public static final int  ERROR_NOT_ENOUGH_MEMORY= 5;

    /**
     * ERROR: No todas las filas tienen igual cantidad de columnas.
     *
     */
    public static final int  ERROR_NOT_EQUALS_ROWS= 6;

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
     * Construye una instancia de <code>TOLMatrixException</code> con el código y mensaje de error especificados.
     * @param errorCode Código del error que provocó que se lanzara la excepción.
     * @param msg mensaje describiendo el error que provocó que se lanzara la excepción.
     */
    public TOLMatrixException(int errorCode, String msg) {
        super(msg);
        this.errorCode= errorCode;
    }
}
