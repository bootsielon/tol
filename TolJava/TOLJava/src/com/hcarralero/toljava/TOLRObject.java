
package com.hcarralero.toljava;

/**
 * <p>Representa un objeto definido en un entorno de TOL, la máquina de TOL es quien
 * maneja al objeto, en cuanto a su creación, generación de valores y eliminación.
 * El objeto en TOLJava sólo es una referencia a un objeto definido en TOL.</p>
 *
 * <p>Subclases de <code>TOLRObject</code> deben tener correspondencia con tipos de datos
 * de TOL, tales como TimeSet, Set y Serie.</p>
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public abstract class TOLRObject implements TOLObject {

    protected TOLMachine tm;

    /**
     * Obtiene el objeto TOLMachine al que está ligado esta referencia
     *
     * @return el objeto TOLMachine al que está ligado esta referencia
     */
    public TOLMachine getTm() {
        return tm;
    }

    /**
     * Establece el objeto <code>TOLMachine</code> al que está ligado esta referencia
     *
     * @param tm nuevo objeto <code>TOLMachine</code>
     */
    public void setTm(TOLMachine tm) {
        this.tm = tm;
    }

    protected String varname;

    /**
     * Obtiene el nombre de la variable del objeto en TOL
     *
     * @return el nombre de la variable del objeto en TOL
     */
    public String getVarname() {
        return varname;
    }

    /**
     * Establece el nombre de la variable del objeto en TOL
     *
     * @param varname nuevo nombre de la variable
     */
    public void setVarname(String varname) {
        this.varname = varname;
    }

    /**
     *  Construye un objeto de tipo <code>TOLRObject</code>
     *
     * @param tm el objeto <code>TOLMachine</code> al que está ligado
     * @param varname el nombre de la variable en TOL
     */
    public TOLRObject(TOLMachine tm, String varname) {
        this.tm = tm;
        this.varname = varname;
    }

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
     * @return <code>false</code> ya que el objeto no corresponde al tipo Matrix de TOL
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
     * @return <code>true</code> ya que el objeto representa una referencia a un objeto creado en TOL
     */
    @Override
    public boolean isReference() {
        return true;
    }

}
