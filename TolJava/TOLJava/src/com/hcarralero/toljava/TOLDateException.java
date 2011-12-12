
package com.hcarralero.toljava;

/**
 * Representa una condición de error ocurrida durante el manejo a un objeto de
 * tipo <code>TOLDate</code>.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLDateException extends Exception {

    /**
     * ERROR: Año NO VÁLIDO para la fecha (<0)
     *
     */
    public static final int  ERROR_INVALID_YEAR= 1;

    /**
     * ERROR: Mes NO VÁLIDO para la fecha (<1|>12)
     *
     */
    public static final int  ERROR_INVALID_MONTH= 2;

    /**
     * ERROR: Día NO VÁLIDO para la fecha (<1|>número de días del mes)
     *
     */
    public static final int  ERROR_INVALID_DAY= 3;

    /**
     * ERROR: Hora NO VÁLIDA para la fecha (<1|>24)
     *
     */
    public static final int  ERROR_INVALID_HOUR= 4;

    /**
     * ERROR: Minuto NO VÁLIDO para la fecha (<1|>60)
     *
     */
    public static final int  ERROR_INVALID_MINUTE= 5;

    /**
     * ERROR: Segundo NO VÁLIDO para la fecha (<1|>60)
     *
     */
    public static final int  ERROR_INVALID_SECOND= 6;

    private int errorCode;

    /**
     * Retorna el código del error que provocó que se lanzara la excepción
     *
     * @return el código del error
     */
    public int getErrorCode() {
        return errorCode;
    }

    /**
     * Construye una instancia de <code>TOLDateException</code> con el código y mensaje de error especificados
     * @param errorCode Código del error que provocó que se lanzara la excepción
     * @param msg mensaje describiendo el error que provocó que se lanzara la excepción
     */
    public TOLDateException(int errorCode, String msg) {
        super(msg);
        this.errorCode= errorCode;
    }

}
