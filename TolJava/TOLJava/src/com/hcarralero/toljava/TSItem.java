
package com.hcarralero.toljava;

/**
 * Representa a una item de una serie, es un par compuesto por una fecha y un valor
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLRSerie
 */
public class TSItem {

    /**
     * Fecha
     */
    protected TOLDate date;

    /**
     * Obtiene la fecha
     *
     * @return la fecha
     */
    public TOLDate getDate() {
        return date;
    }

    /**
     * Establece una nueva fecha.
     *
     * @param date nueva fecha
     */
    public void setDate(TOLDate date) {
        this.date = date;
    }
    /**
     * Valor
     */
    protected double value;

    /**
     * Obtiene el valor
     *
     * @return el valor
     */
    public double getValue() {
        return value;
    }

    /**
     * Establece un nuevo valor
     *
     * @param value nuevo valor
     */
    public void setValue(double value) {
        this.value = value;
    }

    /**
     * Construye un nuevo item de serie (<code>TSItem</code>)
     *
     * @param date fecha
     * @param value valor
     */
    public TSItem(TOLDate date, double value) {
        this.date = date;
        this.value = value;
    }

}
