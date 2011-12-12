
package com.hcarralero.toljava;

/**
 * <p>Representa un valor de los admitidos por TOL.</p>
 * <p>Subclases de <code>TOLLObject</code> deben tener correspondencia con tipos de datos
 * de TOL, tales como Text, Real, Date, etc.</p>
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public abstract class TOLLObject implements TOLObject {

    /**
     * Determina si el objeto corresponde al tipo Text de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Text de TOL
     */
    @Override
    public boolean isText() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Real de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Real de TOL
     */
    @Override
    public boolean isReal() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Date de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Date de TOL
     */
    @Override
    public boolean isDate() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Complex de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Complex de TOL
     */
    @Override
    public boolean isComplex() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Matrix de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Matrix de TOL.
     */
    @Override
    public boolean isMatrix() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo VMatrix de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo VMatrix de TOL
     */
    @Override
    public boolean isVMatrix() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Polyn de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Polyn de TOL
     */
    @Override
    public boolean isPolyn() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Ratio de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Ratio de TOL
     */
    @Override
    public boolean isRatio() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Serie de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Serie de TOL
     */
    @Override
    public boolean isSerie() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo TimeSet de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo TimeSet de TOL
     */
    @Override
    public boolean isTimeSet() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Set de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Set de TOL
     */
    @Override
    public boolean isSet() {
        return false;
    }

    /**
     * Determina si el objeto corresponde al tipo Code de TOL
     *
     * @return <code>false</code> ya que el objeto no corresponde al tipo Code de TOL
     */
    @Override
    public boolean isCode() {
        return false;
    }

    /**
     * Determina si el objeto representa una referencia a un objeto creado en TOL
     *
     * @return <code>false</code> ya que el objeto no representa una referencia a un objeto creado en TOL
     */
    @Override
    public boolean isReference() {
        return false;
    }

}
