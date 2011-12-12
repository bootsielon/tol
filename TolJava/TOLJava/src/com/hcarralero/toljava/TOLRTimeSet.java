
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
public class TOLRTimeSet extends TOLRObject {

    /**
     *  Construye un objeto de tipo <code>TOLRObject</code>.
     * 
     * @param tm el objeto <code>TOLMachine</code> al que está ligado.
     * @param varname nombre de la variable en TOL.
     */
    public TOLRTimeSet(TOLMachine tm, String varname) {
        super(tm, varname);
    }

    /**
     * Determina si el objeto corresponde al tipo TimeSet de TOL.
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo TimeSet de TOL.
     */
    @Override
    public boolean isTimeSet() {
        return true;
    }

    /**
     * Obtiene la fecha sucesora a la indicada en un <code>TimeSet</code>.
     *
     * @param date fecha de la cual se desea su sucesora.
     * @return la fecha sucesora a la indicada en un <code>TimeSet</code>.
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL.
     */
    public TOLDate getNextDate(TOLDate date) throws TOLMachineException {
        return tm.getTimeSetNextDate(varname, date);
    }

    /**
     * Obtiene la fecha antecesora a la indicada en un <code>TimeSet</code>.
     *
     * @param date fecha de la cual se desea su antecesora.
     * @return la fecha antecesora a la indicada en un <code>TimeSet</code>.
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL.
     */
    public TOLDate getPrevDate(TOLDate date) throws TOLMachineException {
        return tm.getTimeSetPrevDate(varname, date);
    }

    /**
     * Obtiene un arreglo de fechas pertenecientes al <code>TimeSet</code> a partir de una dada.
     *
     * @param date Fecha inicial del arreglo.
     * @param size Tamaño del arreglo.
     * @return el arreglo de fechas pertenecientes al <code>TimeSet</code> a partir de una dada.
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL.
     */
    public TOLDate[] getTimeSetDates(TOLDate date, int size) throws TOLMachineException {
        return tm.getTimeSetDates(varname, date, size);
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLRTimeSet</code> y contiene un TimeSet idéntico
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLRTimeSet other = (TOLRTimeSet) obj;
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
     * Obtiene una representación como cadena de texto del TimeSet en la que incluye el nombre
     *
     * @return la representación como cadena de texto del TimeSet en la que incluye el nombre
     */
    @Override
    public String toString() {
        return "TimeSet@"+varname;
    }

}
