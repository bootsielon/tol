package com.hcarralero.toljava;

import java.util.Iterator;
import java.util.Vector;

/**
 * Representa a un objeto TOL de tipo Set
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLSet extends TOLLObject {

    private Vector set= new Vector();

    /**
     * Get the value of set
     *
     * @return the value of set
     */
    public Vector getSet() {
        return set;
    }

    /**
     * Set the value of set
     *
     * @param set new value of set
     */
    public void setSet(Vector set) {
        this.set = set;
    }

    /**
     *  Construye un objeto de tipo <code>TOLRSet</code>
     *
     */
    public TOLSet() {
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
        return this.set.size();
    }

    /**
     * Obtiene el elemento ubicado en la posición dada
     *
     * @param index Posición del elemento
     * @return el elemento ubicado en la posición dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLObject getElement(int index) throws TOLMachineException {
        if (index<1||index>this.set.size()) {
            throw new TOLMachineException(TOLMachineException.ERROR_INVALID_INDEX, "Índice fuera de rango");
        }
        return (TOLObject)this.set.get(index-1);
    }

    /**
     * Obtiene un arreglo con todos los elementos del Set
     *
     * @return un arreglo con todos los elementos del Set
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLObject[] getElements() throws TOLMachineException {
        TOLObject eles[]= new TOLObject[0];
        return (TOLObject [])this.set.toArray(eles);
    }

    /**
     *
     * @param ele
     * @return
     */
    public boolean addElement(TOLObject ele) {
        return this.set.add(ele);
    }

    /**
     *
     * @param ele
     * @return
     */
    public boolean removeElement(TOLObject ele) {
        return this.set.remove(ele);
    }

    /**
     *
     * @param index
     * @return
     */
    public TOLObject removeElement(int index) {
        return (TOLObject)this.set.remove(index);
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
        final TOLSet other = (TOLSet) obj;
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
        String res="[[";
        boolean coma= false;
        Iterator<TOLObject> itset= set.iterator();
        while (itset.hasNext()) {
            TOLObject to= itset.next();
            if (coma) {
                res+=",";
            } else {
                coma= true;
            }
            res+= to.toString();
        }
        res+="]]";
        return res;
    }

}
