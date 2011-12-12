
package com.hcarralero.toljava;

/**
 * Representa una condición de error ocurrida durante la manipulación de un
 * objeto de tipo <code>TOLSet</code>.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLSetException extends Exception {

    /**
     * No ha ocurrido ningún error (¡IMPOSIBLE!)
     *
     */
    public static final int NO_ERROR = 0;

    /**
     * ERROR: Se intenta añadir un valor NULO al conjunto
     *
     */
    public static final int  ERROR_NULL_OBJECT= 1;

    /**
     * ERROR: Error interno de JVM al realizar una operación
     *
     */
    public static final int  ERROR_INTERNAL_JVM_ERROR= 2;

    /**
     * ERROR: Se intenta acceder a un elemento con índice fuera de rango (<0 o mayor que la capacidad del conjunto)
     *
     */
    public static final int  ERROR_INDEX_OUT_OF_BOUND= 3;

    private int errorCode = NO_ERROR;

    /**
     * Retorna el código del error que provocó que se lanzara la excepción.
     *
     * @return el código del error
     */
    public int getErrorCode() {
        return errorCode;
    }

    /**
     * Construye una instancia de <code>TOLSetException</code> con el código y mensaje de error especificados
     * @param errorCode Código del error que provocó que se lanzara la excepción
     * @param msg mensaje describiendo el error que provocó que se lanzara la excepción
     */
    public TOLSetException(int errorCode, String msg) {
        super(msg);
        this.errorCode= errorCode;
    }
    
}
