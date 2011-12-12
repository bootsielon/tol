
package com.hcarralero.toljava;

/**
 * <p>Representa un valor de los admitidos por TOL.</p>
 * <p>Clases que implementan <code>TOLObject</code> deben tener correspondencia con tipos de datos
 * de TOL, tales como Text, Real, Date, etc.</p>
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public interface TOLObject {

    /**
     * Determina si el objeto corresponde al tipo Text de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Text de TOL, <code>false</code> en caso contrario.
     */
    public boolean isText();

    /**
     * Determina si el objeto corresponde al tipo Real de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Real de TOL, <code>false</code> en caso contrario.
     */
    public boolean isReal();

    /**
     * Determina si el objeto corresponde al tipo Date de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Date de TOL, <code>false</code> en caso contrario.
     */
    public boolean isDate();

    /**
     * Determina si el objeto corresponde al tipo Complex de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Complex de TOL, <code>false</code> en caso contrario.
     */
    public boolean isComplex();

    /**
     * Determina si el objeto corresponde al tipo Matrix de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Matrix de TOL, <code>false</code> en caso contrario.
     */
    public boolean isMatrix();

    /**
     * Determina si el objeto corresponde al tipo VMatrix de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo VMatrix de TOL, <code>false</code> en caso contrario.
     */
    public boolean isVMatrix();

    /**
     * Determina si el objeto corresponde al tipo Set de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Set de TOL, <code>false</code> en caso contrario.
     */
    public boolean isSet();

    /**
     * Determina si el objeto corresponde al tipo Serie de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Serie de TOL, <code>false</code> en caso contrario.
     */
    public boolean isSerie();

    /**
     * Determina si el objeto corresponde al tipo TimeSet de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo TimeSet de TOL, <code>false</code> en caso contrario.
     */
    public boolean isTimeSet();

    /**
     * Determina si el objeto corresponde al tipo Polyn de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Polyn de TOL, <code>false</code> en caso contrario.
     */
    public boolean isPolyn();

    /**
     * Determina si el objeto corresponde al tipo Ratio de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Ratio de TOL, <code>false</code> en caso contrario.
     */
    public boolean isRatio();

    /**
     * Determina si el objeto corresponde al tipo Code de TOL.
     *
     * @return <code>true</code> si el objeto corresponde al tipo Code de TOL, <code>false</code> en caso contrario.
     */
    public boolean isCode();

    /**
     * Determina si el objeto es una referencia a un objeto creado en TOL.
     *
     * @return <code>true</code> si el objeto representa una referencia a uno creado en TOL, <code>false</code> en caso contrario.
     */
    public boolean isReference();

}
