package com.hcarralero.toljava;

/**
 * Representa a un objeto TOL de tipo Set
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLRSet extends TOLRObject {

    /**
     *  Construye un objeto de tipo <code>TOLRSet</code>
     *
     * @param tm el objeto <code>TOLMachine</code> al que está ligado
     * @param varname nombre de la variable en TOL
     */
    public TOLRSet(TOLMachine tm, String varname) {
        super(tm, varname);
    }

    /**
     * Determina si el objeto corresponde al tipo Set de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Set de TOL
     */
    @Override
    public boolean isSet() {
        return true;
    }

    /**
     * Obtiene el tamaño del Set
     *
     * @return el tamaño del Set
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public int getSize() throws TOLMachineException {
        return this.tm.getSetSize(varname);
    }

    /**
     * Obtiene el elemento ubicado en la posición dada
     *
     * @param index Posición del elemento
     * @return el elemento ubicado en la posición dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLObject getElement(int index) throws TOLMachineException {
        return this.tm.getSetElement(this.varname, index);
    }

    /**
     * Obtiene un arreglo con todos los elementos del Set
     *
     * @return un arreglo con todos los elementos del Set
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLObject[] getElements() throws TOLMachineException {
        return this.tm.getSetElements(this.varname);
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLRSet</code> y contiene un conjunto idéntica
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLRSet other = (TOLRSet) obj;
        return true;
    }

    /**
     * Calcula el código hash asignado al objeto
     *
     * @return el código hash asignado al objeto
     */
    @Override
    public int hashCode() {
        int hash = 7;
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del Set en la que incluye el nombre
     *
     * @return la representación como cadena de texto del Set en la que incluye el nombre
     */
    @Override
    public String toString() {
        return "Set@"+varname;
    }

}
